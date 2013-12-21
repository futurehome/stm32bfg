/*
  ******************************************************************************
  * @file    usb_endp.c
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   Usb end point routines prototypes
  ******************************************************************************
*/


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
//#include "hw_config.h"
#include "bf_simu_platform.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_endp.h"
#include "usb_core.h"
#include "bf_general.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

u8 	cFlagUsbDataReceived;
u8	cFlagUsbIdle;
u8	cFlagEp0InBusy;
u8	cFlagEp1InBusy;
u8	cFlagEp0LastIn;
u8	cFlagEp1LastIn;



extern  uint8_t USART_Rx_Buffer[];
extern uint32_t USART_Rx_ptr_out;
extern uint32_t USART_Rx_length;
extern uint8_t  USB_Tx_State;
extern	u8	cUsbDeviceStatus[2];
extern	u8	cUsbInterfaceStatus[2];
extern	u8	cUsbEndpointStatus[2];
extern	u8	cUsbCurrentConfiguration;
extern	u8	cUsbCurrentInterface;
extern	u8	cFlagHostCommandDataReceived;
extern	u8	cFlagEp0InStatus;
extern	u8	cIdleData[0x2];
extern	u8	cCom2RxBuffer[USART_RX_DATA_SIZE]; 
extern	u8	cCom2TxBuffer[USART_TX_DATA_SIZE];
extern	u16	wCom2TxBufferStart;
extern	u16	wCom2TxBufferEnd;
extern	u16	wCom2RxBufferStart;
extern	u16	wCom2RxBufferEnd;
extern u16 * wpComRegister;




u8	cFlagUsbReceiveBufferMode;
u8	cFlagReceivePending;
u16 	wReceiveLoopBufferStart;
u16 	wReceiveLoopBufferEnd;
u16 	wSendLoopBufferStart;
u16 	wSendLoopBufferEnd;
u32 	dwReceivedBytes;
u32	dwSofCount;




u8 	cUsbReceiveLoopBuffer[USB_LOOP_BUFFER_SIZE];
u8 	cUsbSendLoopBuffer[USB_LOOP_BUFFER_SIZE];
u8	cEp0OutBuffer[ENDP0_MAX_PACKET_SIZE];
u16	wEp0InLength;
u16	wEp0OutLength;
u8 *	cpEp0InCurrentPointer;
u8	cFlagEp0Setup;
u8	cFlagEp0Out;
u8	cFlagEp0InStall;
u8	cFlagUsbEp1Send;
u8	cFlagUsbEp0Send;
u8	cFlagUsbTxBusy;
u8	cFlagUsbInfo;		//bit0:Data received; bit1: tx buffer not full; bit 2: suspend; bit 3:usb configered

USB_INT_EVENT UsbEvent;
USB_EP_PROCESS UsbEpProcess;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback (void)
{
	u16 wLength;
	
	if(wSendLoopBufferEnd == wSendLoopBufferStart)
	{
		cFlagUsbTxBusy = 0;
	}
	else
	{
		cFlagUsbTxBusy = 1;
		Ep1FillKeepAliveFlag();		//for FT232HL, first 2 bytes should be flag
		wLength = GetUsbTxBufferDataLength();
		if(wLength > MAX_USB_DATA_PACKET_SIZE - 2)		//calc proper length to fill a packet
		//if(wLength > MAX_USB_DATA_PACKET_SIZE)		//calc proper length to fill a packet
		{
			wLength = MAX_USB_DATA_PACKET_SIZE - 2;
		}
		Ep1FillUsbPacket(wLength);
		cFlagUsbIdle = 0;			//reset usb tx idle timer and flag
		dwSofCount = 0;
		wLength += 2;				//need add FT232HL flag byte length
		SetUsbSend(ENDP1, wLength);
	}
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP2_OUT_Callback(void)
{
  	UsbEvent.cFlagUsbEventEp2Out = 1;
}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SOF_Callback(void)
{
  
	if(bDeviceState == CONFIGURED)
	{
		if (dwSofCount++ == USB_IDLE_INTERVAL)
		{
			/* Reset the idle counter */
			//dwSofCount = 0;
      
			/* When SOF count reachs the interval, mark the idle flag. When BF function starts 
				a USB traffic, it will clear the sof counter and idle flag.*/
			//cFlagUsbIdle = 1;
			//UsbEvent.cFlagUsbEventSof = 1;
			UsbWrite((u8*)cIdleData, 0);
		}
	}  
}


/*******************************************************************************
* Function Name  : CopyUsbPacketToReceiveLoopBuffer
* Description    : Copy received usb data from PMA buffer to user buffer, 
*				the data will be appended to the loop buffer end pointer. 
*				In BUFF_WAITING mode, If the loop buffer rest size is smaller than the received data in PMA,
*					copy op will pending, NAK the end point till the loop buffer has enough space to fit the packet.
*				In BUFF_OVERTURN mode, copy will keep going, even the end pointer meet start point. 
					when overturn occurs, some data at start pointer will be overwrited.
* Input          : Selected end point.
* Output         : None.
* Return         : The length of copied data to loop buffer
*******************************************************************************/
u16 CopyUsbPacketToReceiveLoopBuffer(u8 ucEndpointNumber)
{
	u16 wDataLength = 0;		//initial copied data length
	u32 i, n;
	u32 * dwpPMABufferPointer;	//USB PMA buffer of the selected end point
	u16 wTemp;
	//u16 wComCR1;

  	/* Get the number of received data on the selected Endpoint */
  	wDataLength = GetEPRxCount(ucEndpointNumber);

	/* In BUFF_WAITING mode, if loop buffer is not enough, pending receiving and NAK selected endpoint at caller.
		To fix it, enlarge the size of receive loop buffer, or move out the data in loop buffer soon to clear the buffer.*/
	/* start == end, means buffer empty*/

	if(wDataLength == 0)
	{
		return 1;		//wDataLength=0 means empty usb packet, return 1, not zero indicate copy success
	}
	
	if(cFlagUsbReceiveBufferMode == BUFF_WAITING)
	{
		if(wDataLength > GetUsbRxBufferAvailableLength())  //if buff is not enough, pending
		{
			return 0;	//in waiting mode, return 0 means not enough RX buffer, so usb will pending
		}
	}
	
  	/* Get the received data address */
	dwpPMABufferPointer = (u32 *)(((u32)GetEPRxAddr(ucEndpointNumber)) * 2 + PMAAddr );

	n = (wDataLength + 1) >> 1;		//usb ip is 16bit databus width, read/write should be word access each time
	for (i = n; i != 0; i--)
	{
		wTemp = * dwpPMABufferPointer;		//fetch a word
		dwpPMABufferPointer ++;

		/* Store the lower byte and adjust the end pointer*/
		* (u8*)(cUsbReceiveLoopBuffer + wReceiveLoopBufferEnd) = (u8)wTemp;
		wReceiveLoopBufferEnd ++;
		if(wReceiveLoopBufferEnd == USB_LOOP_BUFFER_SIZE)
		{
			wReceiveLoopBufferEnd = 0;
		}

		/* Adding debug code, all usb rx data forward to Com2 tx, if Com2 tx buffer full, skip forward*/
		/*
		if(wCom2TxBufferStart < wCom2TxBufferEnd)
		{
			if(wCom2TxBufferEnd - wCom2TxBufferStart < USART_TX_DATA_SIZE - 1)
			{
				*((u8*)(cCom2TxBuffer + wCom2TxBufferEnd)) = (u8) wTemp;
				wCom2TxBufferEnd ++;
				if(wCom2TxBufferEnd == USART_TX_DATA_SIZE)
				{
					wCom2TxBufferEnd = 0;
				}
			}
		}
		else if(wCom2TxBufferStart > wCom2TxBufferEnd)
		{
			if(wCom2TxBufferStart - wCom2TxBufferEnd > 1)
			{
				*((u8*)(cCom2TxBuffer + wCom2TxBufferEnd)) = (u8) wTemp;
				wCom2TxBufferEnd ++;
				if(wCom2TxBufferEnd == USART_TX_DATA_SIZE)
				{
					wCom2TxBufferEnd = 0;
				}
			}
		}
		else
		{
			*((u8*)(cCom2TxBuffer + wCom2TxBufferEnd)) = (u8) wTemp;
			wCom2TxBufferEnd ++;
			if(wCom2TxBufferEnd == USART_TX_DATA_SIZE)
			{
				wCom2TxBufferEnd = 0;
			}
		}
		*/
		//debug code end
		
		/* If the wDataLength is odd, skip the last byte*/
		if((i == 1) && (wDataLength != n * 2))
		{
			continue;
		}
		else
		{
			/* Store the upper byte and adjust the end pointer*/
			wTemp >>= 8;
			* (u8*)(cUsbReceiveLoopBuffer + wReceiveLoopBufferEnd) = (u8)wTemp;
			wReceiveLoopBufferEnd ++;
			if(wReceiveLoopBufferEnd == USB_LOOP_BUFFER_SIZE)
			{
				wReceiveLoopBufferEnd = 0;
			}
			/* Adding debug code, all usb rx data forward to Com2 tx, if Com2 tx buffer full, skip forward*/
			/*
			if(wCom2TxBufferStart < wCom2TxBufferEnd)
			{
				if(wCom2TxBufferEnd - wCom2TxBufferStart < USART_TX_DATA_SIZE - 1)
				{
					*((u8*)(cCom2TxBuffer + wCom2TxBufferEnd)) = (u8) wTemp;
					wCom2TxBufferEnd ++;
					if(wCom2TxBufferEnd == USART_TX_DATA_SIZE)
					{
						wCom2TxBufferEnd = 0;
					}
				}
			}
			else if(wCom2TxBufferStart > wCom2TxBufferEnd)
			{
				if(wCom2TxBufferStart - wCom2TxBufferEnd > 1)
				{
					*((u8*)(cCom2TxBuffer + wCom2TxBufferEnd)) = (u8) wTemp;
					wCom2TxBufferEnd ++;
					if(wCom2TxBufferEnd == USART_TX_DATA_SIZE)
					{
						wCom2TxBufferEnd = 0;
					}
				}
			}
			else
			{
				*((u8*)(cCom2TxBuffer + wCom2TxBufferEnd)) = (u8) wTemp;
				wCom2TxBufferEnd ++;
				if(wCom2TxBufferEnd == USART_TX_DATA_SIZE)
				{
					wCom2TxBufferEnd = 0;
				}
			}
			*/
			//debug code end
			
		}
	}

	/*Fetch Com2 control register*/
	/*
	wpComRegister = (u16*)(BF_COM2_BASE + USART_REG_CR1);
	wComCR1 = *wpComRegister;

	//if Com2 tx is idle, start to transfer
	if((wComCR1 & USART_REG_CR1_TXEIE) == 0)
	{
		
		wCom2TxBufferStart ++;
		*(u8*)(BF_COM2_BASE + USART_REG_DR) = *(u8*)(cCom2TxBuffer + wCom2TxBufferStart - 1);
		
		if(wCom2TxBufferStart == USART_TX_DATA_SIZE)
		{
			wCom2TxBufferStart = 0;
		}
		wComCR1 |= (USART_REG_CR1_TXEIE);
		* wpComRegister = wComCR1;
	}*/

	/* Return the actual bytes have been copied */
	return wDataLength;

}


/*******************************************************************************
* Function Name  : CopySendLoopBufferToUsbPacket
* Description    : Copy send data from user buffer to PMA buffer , 
*				only the MAX_PACKET_SIZE of selected end point of data can be copied to the PMA. 
* Input          : Selected end point and the data length.
* Output         : None.
* Return         : The length of copied data to PMA, should be <= MAX_PACKET_SIZE, the caller should
*				manage the rest data 
*******************************************************************************/
u16 CopySendLoopBufferToUsbPacket(u8 ucEndpointNumber, u16 wLength)
{
	u16 wCopiedLength = 0;
	u32 i, n;
	u16 wTemp1, wTemp2;
	u16 * wpPMABufferPointer;

	/* start == end, means buffer empty*/
	if(wSendLoopBufferStart != wSendLoopBufferEnd)
	{
		if(wSendLoopBufferStart < wSendLoopBufferEnd)
		{
			
			if((wSendLoopBufferEnd - wSendLoopBufferStart) >= MAX_USB_DATA_PACKET_SIZE)
			{
				wCopiedLength = MAX_USB_DATA_PACKET_SIZE;
			}
			else
			{
				wCopiedLength = wSendLoopBufferEnd - wSendLoopBufferStart;
			}
		}
		else
		{
			if((wSendLoopBufferEnd + USB_LOOP_BUFFER_SIZE - wSendLoopBufferStart) >= MAX_USB_DATA_PACKET_SIZE)
			{
				wCopiedLength = MAX_USB_DATA_PACKET_SIZE;
			}
			else
			{
				wCopiedLength = wSendLoopBufferEnd + USB_LOOP_BUFFER_SIZE - wSendLoopBufferStart;
			}
		}

		n = (wCopiedLength + 1) >> 1;			//usb ip is 16bit databus width, read/write should be word access each time
		wpPMABufferPointer = (u16 *)(GetEPTxAddr(ucEndpointNumber) * 2 + PMAAddr);
		
		for(i = n; i!=0; i--)
		{
			wTemp1 = (u16)(* (cUsbSendLoopBuffer + wSendLoopBufferStart));
			wSendLoopBufferStart ++;
			if(wSendLoopBufferStart == USB_LOOP_BUFFER_SIZE)
			{
				wSendLoopBufferStart = 0;
			}
			wTemp2 = wTemp1 | (((u16)(* (cUsbSendLoopBuffer + wSendLoopBufferStart))) << 8);
			* wpPMABufferPointer = wTemp2;
			wpPMABufferPointer ++;
			wpPMABufferPointer ++;
			if((i == 1) && (wCopiedLength != n*2))
			{
				continue;
			}
			else
			{
				wSendLoopBufferStart ++;
				if(wSendLoopBufferStart == USB_LOOP_BUFFER_SIZE)
				{
					wSendLoopBufferStart = 0;
				}
			}
		}
	}

	return wCopiedLength;
}




/*******************************************************************************
* Function Name  : SetUsbSend
* Description    : Set selected end point register to send packet. 
* Input          : Selected end point and the data length in a packet.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SetUsbSend(u8 ucEndpointNumber, u16 wLength)
{
	SetEPTxCount(ucEndpointNumber, wLength);
	SetEPTxValid(ucEndpointNumber);
}


/*******************************************************************************
* Function Name  : SetUsbReceiveEnable
* Description    : Set selected end point register to open the receive pipe. 
* Input          : Selected end point.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SetUsbReceiveEnable(u8 ucEndpointNumber)
{
	/* Enable the receive of data on the endpoint */
  	SetEPRxValid(ucEndpointNumber);
}


/*******************************************************************************
* Function Name  : UsbEndpointInit
* Description    : Initialize the additional variables for usb end point. 
* Input           : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbEndpointInit(void)
{
	cFlagUsbReceiveBufferMode = BUFF_OVERWRITE;
	cFlagReceivePending = FALSE;
	cFlagUsbIdle = 0;
	dwSofCount = 0;
	wReceiveLoopBufferEnd = 0;
	wReceiveLoopBufferStart = 0;
	wSendLoopBufferEnd = 0;
	wSendLoopBufferStart = 0;
	cFlagEp0Out = 0;
	cFlagEp0Setup = 0;
	cUsbInterfaceStatus[0] = 0;
	cUsbInterfaceStatus[1] = 0;
	cUsbEndpointStatus[0] = 0;
	cUsbEndpointStatus[1] = 0;
	cUsbDeviceStatus[0] = 0;
	cUsbDeviceStatus[1] = 0;
	if(cConfigDescriptor[7] & 0x40)			//if self power, bit0 = 1;
	{
		cUsbDeviceStatus[0] = 0x01;
	}
	cUsbCurrentConfiguration = 0;
	cUsbCurrentInterface = 0;
	cFlagEp0InStall = 0;
	dwReceivedBytes = 0;
//	cFlagUsbEvent = 0;
	bDeviceState = ATTACHED;
	cFlagEp0InBusy = 0;
	cFlagEp1InBusy = 0;
	cFlagEp0LastIn = 1;
	cFlagEp1LastIn = 1;
	UsbEvent.cFlagUsbEventEp0In = 0;
	UsbEvent.cFlagUsbEventEp0Out = 0;
	UsbEvent.cFlagUsbEventEp0Setup = 0;
	UsbEvent.cFlagUsbEventEp2Out = 0;
	cFlagUsbTxBusy = 0;
	cFlagUsbInfo = 0x02;	//no received data; tx buffer available; no suspend; not configured
							//bit0=1, have received data; bit1=1, Tx buffer not full;
							//bit2=1, usb suspended; bit3=1, usb configured;
	cIdleData[0] = 0x01;
	cIdleData[1] = 0x60;

}

void Ep0InProcess(void)
{
	u16 wCopiedLength = 0;
	u32 i, n;
	u16 wTemp1, wTemp2;
	u16 * wpPMABufferPointer;

	if(cFlagUsbEp0Send == USB_EP0_IN_SEND)
	{
		if(wEp0InLength > ENDP0_MAX_PACKET_SIZE)
		{
			wCopiedLength = ENDP0_MAX_PACKET_SIZE;
			cFlagUsbEp0Send = USB_EP0_IN_SEND;
		}
		else 
		{
			wCopiedLength = wEp0InLength;
			//if(pInformation->USBbmRequestType & 0x40)
			//{
			//	cFlagUsbEp0Send = USB_EP0_IN_NAK;
			//}
			//else
			//{
				//cFlagUsbEp0Send = USB_EP0_IN_SEND;
				//if(wEp0InLength == 0)
				//{
					cFlagUsbEp0Send	 = 0;
					if(cFlagEp0InStatus)
					{
						cFlagUsbEp0Send = USB_EP0_IN_SEND;
						cFlagEp0InStatus = 0;
					}
				//}
			//}
		}

		wEp0InLength -= wCopiedLength;

		n = (wCopiedLength + 1) >> 1;			//usb ip is 16bit databus width, read/write should be word access each time
		wpPMABufferPointer = (u16 *)(GetEPTxAddr(ENDP0) * 2 + PMAAddr);
		i = n;
		while(i!=0)
		{
			wTemp1 = (u16)(* cpEp0InCurrentPointer);
			cpEp0InCurrentPointer ++;
			wTemp2 = wTemp1 | (((u16)(* cpEp0InCurrentPointer)) << 8);
			* wpPMABufferPointer = wTemp2;
			wpPMABufferPointer ++;
			wpPMABufferPointer ++;
			if((i == 1) && (wCopiedLength != n*2))
			{
				i--;
				continue;
			}
			else
			{
				cpEp0InCurrentPointer ++;
			}
			i--;
		}
		dwSofCount = 0;
		cFlagUsbEp1Send = 0;
		SetUsbSend(ENDP0, wCopiedLength);
		cFlagEp0InBusy = 1;
	}
	else if(cFlagUsbEp0Send == USB_EP0_IN_STALL)
	{
		cFlagUsbEp0Send = USB_EP0_IN_NAK;
		_SetEPTxStatus(ENDP0, EP_TX_STALL);
	}
	else
	{
		;
	}
}

void Ep0OutProcess(void)
{
	u16 wCopiedLength;
	u32 i, n;
	u16 wTemp;
	u32 * dwpPMABufferPointer;

	wCopiedLength = GetEPRxCount(ENDP0);
	dwpPMABufferPointer = (u32 *)(((u32)GetEPRxAddr(ENDP0)) * 2 + PMAAddr );

	n = (wCopiedLength + 1) >> 1;		//usb ip is 16bit databus width, read/write should be word access each time
	for (i = n; i != 0; i--)
	{
		wTemp = * dwpPMABufferPointer;		//fetch a word
		dwpPMABufferPointer ++;

		/* Store the lower byte and adjust the end pointer*/
		cEp0OutBuffer[i*2] = (u8)wTemp;
		cEp0OutBuffer[i*2+1] = (u8)(wTemp>>8);
	}

	SetUsbReceiveEnable(ENDP0);
}

void SetUsbReceivedStall(u8 ucEndpointNumber)
{
	  _SetEPRxStatus(ucEndpointNumber, EP_RX_STALL);
}

void Ep0SetupProcess(void)
{
	Data_Setup0();
}

void SetEp0InStall(void)
{
	cFlagUsbEp0Send = USB_EP0_IN_STALL;
}


void UsbEventProcess(void)
{
	UsbIntFlagProcess();
	
	if(UsbEpProcess.cEp0In)		// ep0 IN pipe is clear
	{
		cFlagEp0InBusy = 0;
		if(cFlagEp1InBusy == 0)
		{
			if(cFlagUsbEp0Send)				// have data to send in ep0 IN pipe
			{
				UsbEpProcess.cEp0In = 0;		//clear the ep0 IN flag, prepare to fill data and send data and prevent send twice
				Ep0InProcess();
			}
		}
	}
	if(UsbEpProcess.cEp0Out)		// ep0 OUT pipe has data arrived
	{
		UsbEpProcess.cEp0Out = 0;		// prevent access same data twice
		Ep0OutProcess();
	}
	if(UsbEpProcess.cEp0Setup)		// SETUP packet arrived
	{
		UsbEpProcess.cEp0Setup = 0;	//prevent response setup command twice
		Ep0SetupProcess();
	}
	if(UsbEpProcess.cEp2Out)		//ep2 out pipe has data arrived
	{
		u16 wTemp = CopyUsbPacketToReceiveLoopBuffer(EP2_OUT);   //copy PMA to receive buffer, return copied size
		if(wTemp == 0)
		{
			;					//waiting for receive buffer clear
		}
		else						// ep2 out data has been saved in receive buffer
		{
			dwReceivedBytes += wTemp;
			UsbEpProcess.cEp2Out = 0;		//clear flag to prevent access data twice
			cFlagHostCommandDataReceived = 1;
			cFlagUsbInfo |= USB_DATA_RECEIVED;	//set received data flag
			SetUsbReceiveEnable(ENDP2);		//open ep2 out pipe to receive new packet
		}
	}
	
}

void UsbIntFlagProcess(void)
{
	u16 wMaskReserved;

	wMaskReserved = GetCNTR();
	SetCNTR(0x0000);			//mask all usb interrupt
	if(UsbEvent.cFlagUsbEventEp0Setup)
	{
		SetUsbReceiveEnable(ENDP0);
		UsbEpProcess.cEp0Setup = 1;
		UsbEvent.cFlagUsbEventEp0Setup = 0;
	}
	if(UsbEvent.cFlagUsbEventEp0In)
	{
		UsbEpProcess.cEp0In = 1;
		UsbEvent.cFlagUsbEventEp0In = 0;
	}
	if(UsbEvent.cFlagUsbEventEp0Out)
	{
		UsbEpProcess.cEp0Out = 1;
		UsbEvent.cFlagUsbEventEp0Out = 0;
	}
	if(UsbEvent.cFlagUsbEventEp2Out)
	{
		UsbEpProcess.cEp2Out = 1;
		UsbEvent.cFlagUsbEventEp2Out = 0;
	}
	
	SetCNTR(wMaskReserved);		//restore usb interrupt mask
}

u8 cFlagUsbEvent(void)
{
	return (UsbEvent.cFlagUsbEventEp0In + UsbEvent.cFlagUsbEventEp0Out + UsbEvent.cFlagUsbEventEp0Setup 
			+ UsbEvent.cFlagUsbEventEp2Out);
			//+ UsbEpProcess.cEp1In);//+ UsbEpProcess.cEp0In  + UsbEpProcess.cEp0Out + UsbEpProcess.cEp2Out);
}

u16 GetUsbTxBufferDataLength(void)
{
	u16 wLength = 0;

	if(wSendLoopBufferStart <= wSendLoopBufferEnd)
	{
		wLength = wSendLoopBufferEnd - wSendLoopBufferStart;
	}
	else
	{
		wLength = USB_LOOP_BUFFER_SIZE + wSendLoopBufferEnd - wSendLoopBufferStart;
	}
	
	return wLength;
}

u16 GetUsbTxBufferAvailableLength(void)
{
	u16 wLength = 0;

	if(wSendLoopBufferStart <= wSendLoopBufferEnd)
	{
		wLength = (USB_LOOP_BUFFER_SIZE - 1) + wSendLoopBufferStart - wSendLoopBufferEnd;
	}
	else
	{
		wLength = wSendLoopBufferStart - wSendLoopBufferEnd - 1;
	}
	
	return wLength;
}

u16 GetUsbRxBufferDataLength(void)
{
	u16 wLength = 0;

	if(wReceiveLoopBufferStart <= wReceiveLoopBufferEnd)
	{
		wLength = wReceiveLoopBufferEnd - wReceiveLoopBufferStart;
	}
	else
	{
		wLength = USB_LOOP_BUFFER_SIZE + wReceiveLoopBufferEnd - wReceiveLoopBufferStart;
	}
	
	return wLength;
}

u16 GetUsbRxBufferAvailableLength(void)
{
	u16 wLength = 0;

	if(wReceiveLoopBufferStart <= wReceiveLoopBufferEnd)
	{
		wLength = (USB_LOOP_BUFFER_SIZE - 1) + wReceiveLoopBufferStart - wReceiveLoopBufferEnd;
	}
	else
	{
		wLength = wReceiveLoopBufferStart - wReceiveLoopBufferEnd - 1;
	}
	
	return wLength;
}

void Ep1FillKeepAliveFlag(void)
{
	u16 * wpPMABufferPointer;
	u16 wTemp1;

	wpPMABufferPointer = (u16 *)(GetEPTxAddr(ENDP1) * 2 + PMAAddr);
	
	wTemp1 = *((u16*)(cIdleData));						//fill 2 byte of FT232HL status to every IN data at the very first location
	* wpPMABufferPointer = wTemp1;
}

void Ep1FillUsbPacket(u16 wFillLength)
{
	u16 * wpPMABufferPointer;
	u16 wTemp1, wTemp2;
	u32 i, n;

	wpPMABufferPointer = (u16 *)(GetEPTxAddr(ENDP1) * 2 + PMAAddr + 4);	//+4, FT232HL add two bytes of flag at header
	n = (wFillLength + 1) >> 1;			//usb ip is 16bit databus width, read/write should be word access each time
	i = n;
	while(i!=0)
	{
		wTemp1 = (u16)(* (cUsbSendLoopBuffer + wSendLoopBufferStart));
		wSendLoopBufferStart ++;
		if(wSendLoopBufferStart == USB_LOOP_BUFFER_SIZE)
		{
			wSendLoopBufferStart = 0;
		}
		wTemp2 = wTemp1 | (((u16)(* (cUsbSendLoopBuffer + wSendLoopBufferStart))) << 8);
		* wpPMABufferPointer = wTemp2;
		wpPMABufferPointer ++;
		wpPMABufferPointer ++;
		if((i == 1) && (wFillLength != n*2))
		{
			i--;
			continue;
			
		}
		else
		{
			wSendLoopBufferStart ++;
			if(wSendLoopBufferStart == USB_LOOP_BUFFER_SIZE)
			{
				wSendLoopBufferStart = 0;
			}
		}
		i--;
	}
}

