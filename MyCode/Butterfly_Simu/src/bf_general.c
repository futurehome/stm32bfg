/*
  ******************************************************************************
  * @file    bf_general.c
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    04-Oct-2013
  * @brief   general routines prototypes in BF project
  ******************************************************************************
*/

#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
//#include "hw_config.h"
#include "bf_simu_platform.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_endp.h"

#include "bf_peripheral_uart.h"
#include "bf_peripheral_spi.h"
#include "bf_peripheral_adc.h"


#include "bf_general.h"
#include "std_defs.h"
#include "HostInteractionProtocols.h"
#include "bf_peripheral_timer.h"



/* Private variables ---------------------------------------------------------*/
extern	u16 	wSendLoopBufferStart;
extern	u16	wSendLoopBufferEnd;
extern	u16 	wReceiveLoopBufferStart;
extern	u16 	wReceiveLoopBufferEnd;
extern	u32 	dwReceivedBytes;
extern	u8	cUsbSendLoopBuffer[USB_LOOP_BUFFER_SIZE];
extern	u8 	cUsbReceiveLoopBuffer[USB_LOOP_BUFFER_SIZE];
extern 	u8 	cIdleData[0x2];
extern u8		cFlagUsbIdle;
extern u32	dwSofCount;
extern	u8	cFlagHostCommandDataReceived;
extern u8	cFlagUsbEp1Send;
extern	u8	cCom2RxBuffer[USART_RX_DATA_SIZE]; 
extern	u8	cCom2TxBuffer[USART_TX_DATA_SIZE];
extern	u16	wCom2TxBufferStart;
extern	u16	wCom2TxBufferEnd;
extern	u16	wCom2RxBufferStart;
extern	u16	wCom2RxBufferEnd;
extern	u16 * wpComRegister;
extern	u8	cFlagUsbTxBusy;
extern	u8	cFlagCom2DataReceived;
extern	u8	cFlagUsbInfo;
extern 	u16 wAdcFilteredResult[BF_ADC_CHANNEL_NUMBER];
extern u8  cFlagHostCommandDataReceived;
extern u32 dwTotalSendBytes;
extern BF_USB_PROTOCOL_OUT_PIPE_DATA_BELONGS_Def UsbOutDataBelongs;
//extern __ ChipMiningStatus[TOTAL_CHIPS_INSTALLED];
extern SOFT_TIMER ChipSoftTimer[TOTAL_CHIPS_INSTALLED];
extern u8 cGoodEngineCount[TOTAL_CHIPS_INSTALLED];

u32	dwCom2TransferSize = 0;
u32	dwUsbSendSize = 0;
char sz_cmd[1024];
u8 	cUsbConfigured;
u8  cUsbTxUnfinished;
__CHIP_PROCESSING_STATUS ChipMiningStatus[TOTAL_CHIPS_INSTALLED];
const unsigned char __aux_CharMap[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};




/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : __ARM_UsbWriteData()
* Description    : Copy data to send loop buffer, prepare to send by lower level routine.
*				If return BF_BUFF_SMALL, means send loop buffer size is small, or there are 
*					some data already in loop buffer and rest space is not enough.  To fix it,
*					may enlarge send loop buffer size or split the sending data, MAX data 
*					length should be buffer size -1.
* Input           : buffer pointer ready to send and length.  Source buffer should  be linear buffer.
* Output         : None.
* Return         : Actural size of data copy to usb TX buffer for sending
*******************************************************************************/
u16 UsbWrite(u8* cpSourceBuffer, u16 wSendLength)
{
	u32 i;
	u16 wActualSendLength;
	u16 wLength = 0;

	//calculate proper length to send to usb tx loop buffer
	wActualSendLength = GetUsbTxBufferAvailableLength();
	if(wActualSendLength > wSendLength)
	{
		wActualSendLength = wSendLength;
	}

	/* Copy source data to send loop buffer */
	for(i = wActualSendLength; i != 0; i--)
	{
		*((u8*)(cUsbSendLoopBuffer + wSendLoopBufferEnd)) = * cpSourceBuffer;
		cpSourceBuffer ++;
		wSendLoopBufferEnd ++;
		if(wSendLoopBufferEnd == USB_LOOP_BUFFER_SIZE)	//if overturn
		{
			wSendLoopBufferEnd = 0;
		}
	}
	if(cFlagUsbTxBusy == 0)		//Usb tx is idle, need start first packet; if not, only copy data
	{
		cFlagUsbTxBusy = 1;
		Ep1FillKeepAliveFlag();		//for FT232HL, first 2 bytes should be flag
		wLength = GetUsbTxBufferDataLength();
		if(wLength > MAX_USB_DATA_PACKET_SIZE - 2)		//calc proper length to fill a packet
		//if(wLength > MAX_USB_DATA_PACKET_SIZE )		//calc proper length to fill a packet
		{
			wLength = MAX_USB_DATA_PACKET_SIZE - 2;
			//wLength = MAX_USB_DATA_PACKET_SIZE ;
		}
		Ep1FillUsbPacket(wLength);
		cFlagUsbIdle = 0;			//reset usb tx idle timer and flag
		dwSofCount = 0;
		wLength += 2;				//need add FT232HL flag byte length
		SetUsbSend(ENDP1, wLength);
	}
	
	
	return wActualSendLength;
}


/*******************************************************************************
* Function Name  : MemoryCopy()
* Description    : Copy source data to target buffer
* Input           : buffer pointer of source and target,  data length to be copied.
* Output         : None.
* Return         : actual length of copied
*******************************************************************************/
u8 MemoryCopy(u8 * cpSourceBuffer, u8 * cpTargetBuffer, u16 wLength)
{
	u16 wCopiedLength = 0;
	u32 i;

	for(i = wLength; i != 0; i--)
	{
		* cpTargetBuffer = * cpSourceBuffer;
		wCopiedLength ++;
	}

	return wCopiedLength;
}

void HostCommandDataProcess(void)
{
	u16 wUsbReceivedDataLength;


	//u16	wCom2TxAvailableLength;
	//u16 wTransferLength;
	//u16 wActualTransferLengthOfUsb2Com2 = 0;

	/* get usb received data length */
	wUsbReceivedDataLength = GetUsbRxBufferDataLength();

	dwCom2TransferSize += wUsbReceivedDataLength;
	//wActualTransferLengthOfUsb2Com2 = UsbToComTransfer(COM2, wUsbReceivedDataLength);
}

void HostCommandErrorProcess(u16 wErrorCode)
{

}

void UsbSendBufferProcess(u8 ucEndpointNumber)
{
	u16 wSendLength;
	u16 * wpPMABufferPointer;
	u16 wTemp1, wTemp2;
	u32 i, n;

	wpPMABufferPointer = (u16 *)(GetEPTxAddr(ucEndpointNumber) * 2 + PMAAddr);
	
	wTemp1 = (u16)(cIdleData[1]);						//fill 2 byte of FT232HL status to every IN data at the very first location
	wTemp2 = (wTemp1<<8) | ((u16)(cIdleData[0])) ;
	* wpPMABufferPointer = wTemp2;
	wpPMABufferPointer ++;
	wpPMABufferPointer ++;
	wSendLength = 2;

	/* calc the wSendLength */
	if(cFlagUsbIdle == 0)
	{
		if(wSendLoopBufferEnd > wSendLoopBufferStart)
		{
			wSendLength = wSendLoopBufferEnd - wSendLoopBufferStart;
			if(wSendLength > (MAX_USB_DATA_PACKET_SIZE - 2))
			{
				wSendLength = MAX_USB_DATA_PACKET_SIZE - 2;
				cFlagUsbEp1Send = 1;						//if need multi-packet to send, set send data flag true for the next transaction
			}
			else
			{
				cFlagUsbEp1Send = 0;	
			}
		}
		else if(wSendLoopBufferEnd < wSendLoopBufferStart)
		{
			wSendLength = wSendLoopBufferEnd - wSendLoopBufferStart + USB_LOOP_BUFFER_SIZE;
			if(wSendLength > (MAX_USB_DATA_PACKET_SIZE - 2))
			{
				wSendLength = MAX_USB_DATA_PACKET_SIZE - 2;
				cFlagUsbEp1Send = 1;						//if need multi-packet to send, set send data flag true for the next transaction
			}
			else
			{
				cFlagUsbEp1Send = 0;	
			}
		}
		else
		{
			wSendLength = 0;
			cFlagUsbEp1Send = 0;	
		}

		n = (wSendLength + 1) >> 1;			//usb ip is 16bit databus width, read/write should be word access each time
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
			if((i == 1) && (wSendLength != n*2))
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
		dwUsbSendSize += wSendLength;
		wSendLength += 2;
	}
	
	cFlagUsbIdle = 0;
	dwSofCount = 0;
	
	SetUsbSend(ENDP1, wSendLength);
	
}

u8 UsbLoopSend(u16 wLength)
{
	u32 i;

	/* Check if the rest space of send buffer is enough */
	if(wSendLoopBufferEnd < wSendLoopBufferStart)
	{
		if((wSendLoopBufferStart - wSendLoopBufferEnd) <= wLength)
		{
			return BF_BUFF_SMALL;
		}
	}
	else if(wSendLoopBufferEnd > wSendLoopBufferStart)
	{
		if((wSendLoopBufferStart + USB_LOOP_BUFFER_SIZE - wSendLoopBufferEnd) <= wLength)
		{
			return BF_BUFF_SMALL;
		}
	}
	else
	{
		if(USB_LOOP_BUFFER_SIZE <= wLength)
		{
			return BF_BUFF_SMALL;
		}
	}

	/* Copy source data to send loop buffer */
	for(i = wLength; i != 0; i--)
	{
		*((u8*)(cUsbSendLoopBuffer + wSendLoopBufferEnd)) = * ((u8*)(cUsbReceiveLoopBuffer + wReceiveLoopBufferStart));
		wReceiveLoopBufferStart ++;
		wSendLoopBufferEnd ++;
		if(wSendLoopBufferEnd == USB_LOOP_BUFFER_SIZE)
		{
			wSendLoopBufferEnd = 0;
		}
		if(wReceiveLoopBufferStart == USB_LOOP_BUFFER_SIZE)
		{
			wReceiveLoopBufferStart = 0;
		}
	}
	cFlagUsbEp1Send = 1;
	
	
	return BF_SUCCESS;
}


/*Transfer Com2 received data to USB tx for loop or debug test*/
void UartCom2RxProcess(void)
{
	u16 wCom2RxDataLength;
	//u16 wUsbTxAvailableBufferLength;
	u16 wTransferLength;
	u16 wUsbIntMaskReserved;
	u16 wActualTransferLength;

	
	/*get usb free send buffer length */
	//wUsbTxAvailableBufferLength = GetUsbTxBufferAvailableLength();

	/* get com2 port data length in receive buffer */
	wCom2RxDataLength = GetComRxBufferDataLength(COM2);


	if((USART_RX_DATA_SIZE - wCom2RxBufferStart) < wCom2RxDataLength)	//if rx buffer overturn, only access data till buffer end
	{
		wTransferLength = USART_RX_DATA_SIZE - wCom2RxBufferStart;
		cFlagCom2DataReceived = TRUE;		//flag for rest data process in main loop
	}
	else
	{
		wTransferLength = wCom2RxDataLength;
	}

	wUsbIntMaskReserved = GetCNTR();
	SetCNTR((~USB_CNTR_SOFM)& wUsbIntMaskReserved);			//mask  usb SOF interrupt to prevent SOF break data copy
	wActualTransferLength = UsbWrite(((u8*) (cCom2RxBuffer + wCom2RxBufferStart)), wTransferLength);
	SetCNTR(wUsbIntMaskReserved);		//restore usb SOF interrupt

	wCom2RxBufferStart += wActualTransferLength;
	if(wCom2RxBufferStart == USART_RX_DATA_SIZE)
	{
		wCom2RxBufferStart = 0;
	}

}


/*	Copy usb rx buffer to com2 tx buffer and start com2 transmitter if it is idle
	return actual copied data length according to MIN(Com2 tx buffer free size, usb rx data size)*/
u16 UsbToComTransfer(COM_TypeDef cComPort, u16 wTransferLength)
{
	u32 i;
	u16 wComCR1;
	u16 wCom2TxAvailableLength = 0;
	u16 wTempUsbRxLoopBufferStart;

	switch(cComPort)
	{
		case COM1:
			break;
		case COM2:
			/*calc the proper length*/
			wCom2TxAvailableLength = GetComAvailableTxBufferLength(COM2);
			if(wCom2TxAvailableLength >= wTransferLength)
			{
				wCom2TxAvailableLength =  wTransferLength;
			}
			wTempUsbRxLoopBufferStart = wReceiveLoopBufferStart;

			/*copy usb buffer to com2 buffer*/
			for(i=wCom2TxAvailableLength; i!=0; i--)
			{
				*((u8*)(cCom2TxBuffer + wCom2TxBufferEnd)) = *(u8*)(cUsbReceiveLoopBuffer + wTempUsbRxLoopBufferStart);
				wTempUsbRxLoopBufferStart ++;
				wCom2TxBufferEnd ++;
				if(wTempUsbRxLoopBufferStart == USB_LOOP_BUFFER_SIZE)
				{
					wTempUsbRxLoopBufferStart = 0;
				}
				if(wCom2TxBufferEnd == USART_TX_DATA_SIZE)
				{
					wCom2TxBufferEnd = 0;
				}
			}

			/*Fetch Com2 control register*/
			wpComRegister = (u16*)(BF_COM2_BASE + USART_REG_CR1);
			wComCR1 = *wpComRegister;

			/*if Com2 tx is idle, start to transfer*/
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
			}
			break;
		case COM3:
			break;
		default:
			break;
	}

	return wCom2TxAvailableLength;
}

u16 ComToUsbTransfer(COM_TypeDef cComPort, u16 wTransferLength)
{
	u32 i;
	u8 cResult = BF_FAIL;
	//u16 wActualTransferLength = 0;


	switch(cComPort)
	{
		case COM1:
			break;
		case COM2:
			//wActualTransferLength = GetUsbTxBufferAvailableLength();
			for(i=wTransferLength; i!=0; i--)
			{
				*((u8*)(cUsbSendLoopBuffer + wSendLoopBufferEnd)) = *(u8*)(cCom2RxBuffer + wCom2RxBufferStart);
				wCom2RxBufferStart ++;
				wSendLoopBufferEnd ++;
				if(wSendLoopBufferEnd == USB_LOOP_BUFFER_SIZE)
				{
					wSendLoopBufferEnd = 0;
				}
				if(wCom2RxBufferStart == USART_TX_DATA_SIZE)
				{
					wCom2RxBufferStart = 0;
				}
			}
			
			cFlagUsbEp1Send = 1;
			cResult = BF_SUCCESS;
			break;
		case COM3:
			break;
		default:
			break;
	}

	return cResult;
}

u16 __ARM_UsbSendData(u8* cpSourceBuffer, u16 wSendLength)
{
	u16 wUsbIntMaskReserved;
	u16 wActuralSendSize;
	u32 dwTimeoutCounter;
	u16 wReturnLength;

	wReturnLength = wSendLength;

	while(1)
	{
		wUsbIntMaskReserved = GetCNTR();
		SetCNTR((~USB_CNTR_SOFM)& wUsbIntMaskReserved);			//mask  usb SOF interrupt
		wActuralSendSize = UsbWrite(cpSourceBuffer, wSendLength);
		SetCNTR(wUsbIntMaskReserved);		//restore usb SOF interrupt

		if(wActuralSendSize < wSendLength)		//have some data left due to usb tx buffer full
		{
			wSendLength -= wActuralSendSize;	//adjust send size = unsend data

			dwTimeoutCounter = 0x0110F0000;		//wait a while,  usb tx buffer will release some space
			while(dwTimeoutCounter > 0)			//loop till tx buff is not full
			{
				if(GetUsbTxBufferAvailableLength() != 0)
				{
					break;
				}
				dwTimeoutCounter--;
			}
			if(dwTimeoutCounter == 0)			//timeout means usb tx jammed, go to function end
			{
				break;
			}
		}
		else
		{
			break;
		}
		
		
	}
	
	return wReturnLength;		//AVR original code always return the expected send length, maybe have issue if usb tx timeout
}

u8 __ARM_USB_GetInformation(void)
{
	return cFlagUsbInfo;
}

void ClearUsbRxBuffer(void)
{
	wReceiveLoopBufferEnd = 0;
	wReceiveLoopBufferStart = 0;
	cFlagUsbInfo &= (~USB_DATA_RECEIVED);
}

u16 __ARM_UsbGetData(u8* cpTargetBuffer, u16 wExpectLength)
{
	u16 wActualGetLength;

	if(wExpectLength == 0) return 0;

	for(wActualGetLength=0; wActualGetLength<wExpectLength; wActualGetLength++)
	{
		cpTargetBuffer[wActualGetLength] = *(u8*)(cUsbReceiveLoopBuffer + wReceiveLoopBufferStart);
		wReceiveLoopBufferStart ++;
		if(wReceiveLoopBufferStart == USB_LOOP_BUFFER_SIZE)
		{
			wReceiveLoopBufferStart = 0;
		}
		if(wReceiveLoopBufferStart == wReceiveLoopBufferEnd)
		{
			cFlagUsbInfo &= (~USB_DATA_RECEIVED);
			break;
		}
	}

	if(wActualGetLength != wExpectLength)
	{
		wActualGetLength++;
	}

	return wActualGetLength;
}

//////////////////////////////////////////////
// SC Chips
//////////////////////////////////////////////
void __ARM_SC_Initialize(void)
{
	SPI_CS_HIGH();
}

void __ARM_ASIC_Activate_CS(void)
{
	SPI_CS_LOW();
}

void __ARM_ASIC_Deactivate_CS(void)
{
	SPI_CS_HIGH();
}

u16 __ARM_SC_WriteData(u8 iChip, u16 iEngine, u8 iAdrs, u16 iData)
{
	unsigned int iCommand = 0;
	u16 wReadData;
	
	// Generate the command
	// RW COMMAND (0 = WRITE)
	iCommand = ((((u16)(iChip)) << 12 ) &  0x7000) | // Chip address
			   ((((u16)(iEngine)) << 8  ) &  0x0F00) |
			   ((((u16)(iAdrs))       ) &  0x00FF); // Memory Address
						  							  
	// Send it via SPI
	//	__MCU_ASIC_Deactivate_CS();
	//__MCU_ASIC_Activate_CS();
	SPI_SendWord(iCommand);
	wReadData = SPI_SendWord(iData);	
	//__MCU_ASIC_Deactivate_CS();
	return wReadData;
}

u16 __ARM_SC_ReadData (u8 iChip, u16 iEngine, u8 iAdrs)
{
	u16 iCommand = 0;
	u16 wTemp;
			
	// Send it via SPI
	// Generate the command
	iCommand = (ASIC_SPI_RW_COMMAND) |  // RW COMMAND (1 = Read)
				((((u16)iChip) << 12 ) &  0x7000) | // Chip address
				((((u16)iEngine ) << 8  ) &  0x0F00) |
				((((u16)iAdrs )        ) &  0x00FF); // Memory Address

	// We're on an 8 chip model
	//		__MCU_ASIC_Deactivate_CS();
	//__MCU_ASIC_Activate_CS();
	SPI_SendWord(iCommand);
	wTemp = SPI_ReadWord();
	//			__MCU_ASIC_Deactivate_CS();
	return wTemp;		
}

void BF_CommandPrepare(void)
{
	u16 umx;
	
	for (umx = 0; umx < 1024; umx++) sz_cmd[umx] = 0;
}

int __ARM_A2D_GetTemp1(void)
{
	//unsigned int a2d_val = 0;
	//float voltage_value;
	//float res;
	u16 wTempReadBack;
	int dwTempCompute;

	/*
	// Activate the channel
	AVR32_A2D_CHANNEL_ENABLE_REGISTER = MAKE_DWORD(0x00,
												   0x00,
												   0x00,
												   (1<<AVR32_A2D_TEMP1_CHANNEL)); // Enables channel 1 by default (CORRECT THIS!)
	// Initiate Conversion
	AVR32_A2D_CONTROL_REGISTER |= (0x02); // Second-Bit activates the START command... thus conversion will start

	// Keep reading until the channel is ready
	

	// Wait until conversion finishes
	while ((AVR32_A2D_CHANNEL_STATUS_REGISTER & MAKE_DWORD(0,0,0,(1<<AVR32_A2D_TEMP1_CHANNEL))) == 0x0); // Wait until the conversion finishes

	// Read the value
	a2d_val = AVR32_A2D_TEMP1_CHANNEL_CDR;

	// Make correct AND operation
	a2d_val &= 0x3FF;

	// Now, the A2D converter will give between 0b0000000000 and 0b1111111111.
	// Each LSB in the A2D range means 3.2 mV. To get the real voltage
	voltage_value = 3.2f * a2d_val;

	// Now we have the voltage.
	// Convert the returned value into temperature
	// Note that 500mV means 0 degrees (that's our offset there...), and each 10mV equals one degree
	res = ((voltage_value - 500) / 10);
	*/

	wTempReadBack = wAdcFilteredResult[BF_TEMP_1];
	dwTempCompute = wTempReadBack * 330 / 4096 - 50 + BF_TEMPERATURE_COMPENSATE;

	if(dwTempCompute < 0) dwTempCompute = 0;
	// We're done
	return (int)dwTempCompute;
}

int __ARM_A2D_GetTemp2(void)
{
	u16 wTempReadBack;
	int dwTempCompute;

	wTempReadBack = wAdcFilteredResult[BF_TEMP_0];
	dwTempCompute = wTempReadBack * 330 / 4096 - 50 + BF_TEMPERATURE_COMPENSATE;

	if(dwTempCompute < 0) dwTempCompute = 0;
	
	// We're done
	return dwTempCompute;
}

void __ARM_FAN_SetSpeed(u8 iSpeed)
{
	/*
	unsigned int iFinalVal;
	unsigned int iMXVal = 0;
	//unsigned int ovr_value;
	//ovr_value = ~( __AVR32_FAN_CTRL3 | __AVR32_FAN_CTRL2 | __AVR32_FAN_CTRL1 | __AVR32_FAN_CTRL0);
	
	iFinalVal = 0;//AVR32_GPIO.port[1].ovr & ovr_value;
	iMXVal = 0;
	
	iMXVal |= (iSpeed & 0x01) ? __AVR32_FAN_CTRL0 : 0;
	iMXVal |= (iSpeed & 0x02) ? __AVR32_FAN_CTRL1 : 0;
	iMXVal |= (iSpeed & 0x04) ? __AVR32_FAN_CTRL2 : 0;
	iMXVal |= (iSpeed & 0x08) ? __AVR32_FAN_CTRL3 : 0;
	iFinalVal |= iMXVal;
	
	//AVR32_GPIO.port[1].ovr = iFinalVal;

	*/
	
}

int __ARM_A2D_Get3P3V(void)
{
	u16 wTempReadBack;
	int voltage_value;

	wTempReadBack = wAdcFilteredResult[BF_ADC_PWR_3V3];
	voltage_value = wTempReadBack * 33 * 2 / 40960;		//ADC in is 3.3V/2
	
	// Now we have the voltage.
	return (int)voltage_value;
}

int __ARM_A2D_Get1V_13(void)
{
	u16 wTempReadBack;
	int voltage_value;

	wTempReadBack = wAdcFilteredResult[BF_ADC_PWR_1V0_13];
	voltage_value = wTempReadBack * 33 / 40960;		// BF Vcore directly connets to ADC in
	
	// Now we have the voltage.
	return (int)voltage_value;
}

int __ARM_A2D_Get1V_02(void)
{
	u16 wTempReadBack;
	int voltage_value;

	wTempReadBack = wAdcFilteredResult[BF_ADC_PWR_1V0_02];
	voltage_value = wTempReadBack * 33 / 40960;		// BF Vcore directly connets to ADC in
	
	// Now we have the voltage.
	return (int)voltage_value;
}

int __ARM_A2D_Get12V(void)
{
	u16 wTempReadBack;
	int voltage_value;

	wTempReadBack = wAdcFilteredResult[BF_ADC_PWR_12V];
	voltage_value = wTempReadBack * 33 * 115 / 40960 /15;		// Main power 12V divided by 10K/1.5K resistor
	
	// Now we have the voltage.
	return (int)voltage_value;
}

void GlobalVariableInitialize(void)
{
	u8 cChip;
	
	USB_Cable_Config(DISABLE);			//Usb soft disconnect
	cUsbConfigured = FALSE;
	cFlagHostCommandDataReceived = FALSE;
	dwTotalSendBytes = 0;
	UsbOutDataBelongs = FREE_DATA;
	ClearSoftTimer0();
	ClearSoftTimer1();
	cUsbTxUnfinished = FALSE;

	for(cChip=0; cChip<TOTAL_CHIPS_INSTALLED; cChip++)
	{
		ChipMiningStatus[cChip].cEngineCount = 0;
		ChipMiningStatus[cChip].cFrequencySlect = __ASIC_FREQUENCY_ACTUAL_INDEX;
		ChipMiningStatus[cChip].dwCalcTimePerChip = 3000;		//set calc time is 30s
		ClearSoftTimerEx(cChip);
		cGoodEngineCount[cChip] = 0;
	}
}

void UartBackDoor(u32 dwData)
{

	
	u8 cTempString[10];
	cTempString[0] = ',';
	cTempString[1] = __aux_CharMap[(dwData >> 28)&0x0F];
	cTempString[2] = __aux_CharMap[(dwData >> 24)&0x0F];
	cTempString[3] = __aux_CharMap[(dwData >> 20)&0x0F];
	cTempString[4] = __aux_CharMap[(dwData >> 16)&0x0F];
	cTempString[5] = __aux_CharMap[(dwData >> 12)&0x0F];
	cTempString[6] = __aux_CharMap[(dwData >> 8 )&0x0F];
	cTempString[7] = __aux_CharMap[(dwData >> 4)&0x0F];
	cTempString[8] = __aux_CharMap[(dwData )& 0x0F];

	ComTransmitData(COM2, cTempString,9);

	
}

void UartBackDoorByte(u8 cData)
{
	u8 cTempString[2];
	cTempString[0] = __aux_CharMap[(cData >> 4)&0x0F];
	cTempString[1] = __aux_CharMap[(cData )& 0x0F];

	ComTransmitData(COM2, cTempString,2);
}

void UartBackDoorWord(u16 wData)
{
	u8 cTempString[4];
	cTempString[0] = __aux_CharMap[(wData >> 12)&0x0F];
	cTempString[1] = __aux_CharMap[(wData >> 8)&0x0F];
	cTempString[2] = __aux_CharMap[(wData >> 4)&0x0F];
	cTempString[3] = __aux_CharMap[(wData )& 0x0F];

	ComTransmitData(COM2, cTempString,4);
}

void GetBFCalculateTime(u8 cChip)	//looks like 3000 = 15s, tested 15s can get result by freq=7
{
	if(ChipMiningStatus[cChip].cEngineCount != 0)
	{
		ChipMiningStatus[cChip].dwCalcTimePerChip = 429400 * 3
		/ ChipMiningStatus[cChip].cEngineCount / __ASIC_FREQUENCY_VALUES[ChipMiningStatus[cChip].cFrequencySlect];
	}
	else
	{
		ChipMiningStatus[cChip].dwCalcTimePerChip = 3000;
	}
}

