/*
  ******************************************************************************
  * @file    bf_peripheral_uart.c
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
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_endp.h"
#include "usb_core.h"
#include "bf_general.h"
#include "bf_simu_platform.h"
#include "bf_simu_peripheral.h"
#include "bf_peripheral_uart.h"



uint32_t USART_Rx_ptr_in = 0;
uint32_t USART_Rx_ptr_out = 0;
uint32_t USART_Rx_length  = 0;

//u32 COM_USART[COMn] = {BF_SIMU_COM1_BASE, BF_SIMU_COM2_BASE}; 

//GPIO_TypeDef* COM_TX_PORT[COMn] = {BF_SIMU_COM1_TX_GPIO_PORT, BF_SIMU_COM2_TX_GPIO_PORT};
 
//GPIO_TypeDef* COM_RX_PORT[COMn] = {BF_SIMU_COM1_RX_GPIO_PORT, BF_SIMU_COM2_RX_GPIO_PORT};
 
//const uint32_t COM_USART_CLK[COMn] = {BF_SIMU_COM1_CLK, BF_SIMU_COM2_CLK};

//const uint32_t COM_TX_PORT_CLK[COMn] = {BF_SIMU_COM1_TX_GPIO_CLK, BF_SIMU_COM2_TX_GPIO_CLK};
 
//const uint32_t COM_RX_PORT_CLK[COMn] = {BF_SIMU_COM1_RX_GPIO_CLK, BF_SIMU_COM2_RX_GPIO_CLK};

//const uint16_t COM_TX_PIN[COMn] = {BF_SIMU_COM1_TX_PIN, BF_SIMU_COM2_TX_PIN};

//const uint16_t COM_RX_PIN[COMn] = {BF_SIMU_COM1_RX_PIN, BF_SIMU_COM2_RX_PIN};

u8	cCom2RxBuffer[USART_RX_DATA_SIZE]; 
u8	cCom2TxBuffer[USART_TX_DATA_SIZE];
u16	wCom2TxBufferStart;
u16	wCom2TxBufferEnd;
u16	wCom2RxBufferStart;
u16	wCom2RxBufferEnd;
u8	cCom2TxEnable;
u8	cCom2TxEvent;
u8	cCom2EventTxEnable;
u8	cFlagCom2DataReceived;
u8	cCom2RxBufferFull;
u16	wCom2RxProcessedLength;
COM_INT_EVENT ComEvent;
COM_INT_EVENT ComProcess;
COM_DATA_MODE ComDataMode;
u16 * wpComRegister;

void ComVariblesInit(void)
{
	wCom2RxBufferEnd = 0;
	wCom2RxBufferStart = 0;
	wCom2TxBufferEnd = 0;
	wCom2TxBufferStart = 0;
	cCom2RxBufferFull = 0;
	ComEvent.cFlagCom1EventRxDataReceived = 0;
	ComEvent.cFlagCom2EventRxDataReceived = 0;
	ComEvent.cFlagCom3EventRxDataReceived = 0;
	ComProcess.cFlagCom1EventRxDataReceived = 0;
	ComProcess.cFlagCom2EventRxDataReceived = 0;
	ComProcess.cFlagCom3EventRxDataReceived = 0;
	wCom2RxProcessedLength = 0;
	ComDataMode = COM_SELF_LOOP;
	cFlagCom2DataReceived = FALSE;
}

void Com2_Isr(void)
{
	u16	wComSR;
	u16	wComCR1;
	//u16 wTempRegValue;
	u8* cpTempComDR;
	u16 wComAvailableRxBufferLength;
	u8	cTemp;

	//wComSR = (u16)(*((u16*)(BF_SIMU_COM2_BASE + USART_REG_SR)));
	wpComRegister = (u16*)(BF_COM2_BASE + USART_REG_SR);
	wComSR = * wpComRegister;
	//wComSR = (u16) dwTempRegValue;
	if((wComSR & USART_FLAG_TXE)!= 0)			//tx enable, DR is empty, auto clear by write to DR register
	{
		if(wCom2TxBufferEnd != wCom2TxBufferStart)
		{
			cpTempComDR = (u8*)(BF_COM2_BASE + USART_REG_DR);
			*cpTempComDR = *((u8*)(cCom2TxBuffer + wCom2TxBufferStart));
			wCom2TxBufferStart ++;
			if(wCom2TxBufferStart == USART_TX_DATA_SIZE)
			{
				wCom2TxBufferStart = 0;
			}
		}
		else
		{
			wpComRegister = (u16*)(BF_COM2_BASE + USART_REG_CR1);
			wComCR1= * wpComRegister;		//when tx buffer empty, disable tx int
			wComCR1 &= (~USART_REG_CR1_TXEIE);
			* wpComRegister = wComCR1;
		}
	}
	if((wComSR & USART_FLAG_RXNE) != 0) 		// rx DR is not empty, SR bit auto clear by read DR register
	{
		wComAvailableRxBufferLength = GetComAvailableRxBufferLength(COM2);
		cTemp = *((u8*)(BF_COM2_BASE + USART_REG_DR));
		if(wComAvailableRxBufferLength == 0)
		{
			cCom2RxBufferFull = 1;
		}
		else
		{
			*((u8*)(cCom2RxBuffer + wCom2RxBufferEnd)) = cTemp;
			wCom2RxBufferEnd ++;
			if(wCom2RxBufferEnd == USART_RX_DATA_SIZE)		//adjust pointer when overturn
			{
				wCom2RxBufferEnd = 0;
			}
			cCom2RxBufferFull = 0;

		}
		cFlagCom2DataReceived = TRUE;
	}
	/*
	if((wComSR & USART_FLAG_TC) != 0)				//check if transmition closed
	{
		wpComRegister = (u16*)(BF_COM2_BASE + USART_REG_CR1);
		wComCR1 = * wpComRegister;		//when tx finished, disable tx close int
		wComCR1 &= (~(USART_REG_CR1_TCIE ));
		* wpComRegister = wComCR1;
	}*/
}


/*
void ComIntFlagProcess(void)
{
	u16 wMaskReserved;

	wMaskReserved = *(u16*)(BF_SIMU_COM2_BASE + USART_REG_CR1);			//save com2 int config to temp variable
	*(u16*)(BF_SIMU_COM2_BASE + USART_REG_CR1) = wMaskReserved & (~USART_REG_CR1_RXNEIE);		//mask rx int	
	if(ComEvent.cFlagCom2EventRxDataReceived)
	{
		ComEvent.cFlagCom2EventRxDataReceived = 0;
		ComProcess.cFlagCom2EventRxDataReceived = 1;
	}
	
	*(u16*)(BF_SIMU_COM2_BASE + USART_REG_CR1) = wMaskReserved;
}*/

u8 cFlagComEvent(void)
{
	return (ComEvent.cFlagCom1EventRxDataReceived + ComEvent.cFlagCom2EventRxDataReceived
			+ ComEvent.cFlagCom3EventRxDataReceived + ComProcess.cFlagCom1EventRxDataReceived
			+ ComProcess.cFlagCom2EventRxDataReceived + ComProcess.cFlagCom3EventRxDataReceived);
}

void ComEventProcess(void)
{
	u16 wLength;
	
	//ComIntFlagProcess();

	if(ComProcess.cFlagCom1EventRxDataReceived)
	{
		ComProcess.cFlagCom1EventRxDataReceived = 0;
		NOP_Process();
	}
	if(ComProcess.cFlagCom2EventRxDataReceived)
	{
		ComProcess.cFlagCom2EventRxDataReceived = 0;
		wLength = GetComRxBufferDataLength(COM2);
		if(wLength !=0)
		{
			//ComToUsbTransfer(COM2, wLength);
		}
	}
	if(ComProcess.cFlagCom3EventRxDataReceived)
	{
		ComProcess.cFlagCom3EventRxDataReceived = 0;
		NOP_Process();
	}
}

u16 ComTransmitData(COM_TypeDef cComPort, u8* cSourcePointer, u16 wTransmitLength)
{
	u16	wActualTransmitLength, i;
	u16	wComCR1;
	

	switch(cComPort)
	{
		case COM2:
			if(wCom2TxBufferEnd > wCom2TxBufferStart)
			{
				if((wCom2TxBufferStart + USART_TX_DATA_SIZE - wCom2TxBufferEnd) > wTransmitLength)
				{
					wActualTransmitLength = wTransmitLength;
				}
				else
				{
					wActualTransmitLength = wCom2TxBufferStart + USART_TX_DATA_SIZE - wCom2TxBufferEnd -1;
				}
			}
			else if(wCom2TxBufferEnd < wCom2TxBufferStart)
			{
				if((wCom2TxBufferStart - wCom2TxBufferEnd) > wTransmitLength)
				{
					wActualTransmitLength = wTransmitLength;
				}
				else
				{
					wActualTransmitLength = wCom2TxBufferStart - wCom2TxBufferEnd - 1;
				}
			}
			else
			{
				if(wTransmitLength <= USART_TX_DATA_SIZE -1)
				{
					wActualTransmitLength = wTransmitLength;
				}
				else
				{
					wActualTransmitLength = USART_TX_DATA_SIZE - 1;
				}
			}
			
			for(i = wActualTransmitLength; i != 0; i--)
			{
				*(u8*)(cCom2TxBuffer + wCom2TxBufferEnd) = * cSourcePointer;
				cSourcePointer ++;
				wCom2TxBufferEnd ++;
				if(wCom2TxBufferEnd == USART_TX_DATA_SIZE)
				{
					wCom2TxBufferEnd = 0;
				}
			}
			if(wCom2TxBufferEnd != wCom2TxBufferStart)
			{
				wpComRegister = (u16*)(BF_COM2_BASE + USART_REG_CR1);
				wComCR1 = *wpComRegister;
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
			}
			break;
		default:
			wActualTransmitLength = 0;
			break;
	}

	return wActualTransmitLength;
}

u16 GetComAvailableRxBufferLength(COM_TypeDef cComPort)
{
	u16 wReceiveBufferLength = 0;

	switch(cComPort)
	{
		case COM1:
			break;
		case COM2:
			if(wCom2RxBufferStart <= wCom2RxBufferEnd)
			{
				wReceiveBufferLength = (USART_RX_DATA_SIZE - 1) + wCom2RxBufferStart - wCom2RxBufferEnd;
			}
			else
			{
				wReceiveBufferLength = wCom2RxBufferStart - wCom2RxBufferEnd - 1;
			}
			break;
		case COM3:
			break;
		default:
			break;
	}
	return wReceiveBufferLength;
}

u16 GetComAvailableTxBufferLength(COM_TypeDef cComPort)
{
	u16 wTransferBufferLength = 0;

	switch(cComPort)
	{
		case COM1:
			break;
		case COM2:
			if(wCom2TxBufferStart <= wCom2TxBufferEnd)
			{
				wTransferBufferLength = (USART_TX_DATA_SIZE - 1) + wCom2TxBufferStart - wCom2TxBufferEnd;
			}
			else
			{
				wTransferBufferLength = wCom2TxBufferStart - wCom2TxBufferEnd - 1;
			}
			break;
		case COM3:
			break;
		default:
			break;
	}

	return wTransferBufferLength;
}

u16 GetComRxBufferDataLength(COM_TypeDef cComPort)
{
	u16 wReceivedBufferLength = 0;

	switch(cComPort)
	{
		case COM1:
			break;
		case COM2:
			if(wCom2RxBufferStart <= wCom2RxBufferEnd)
			{
				wReceivedBufferLength =  wCom2RxBufferEnd - wCom2RxBufferStart;
			}
			else
			{
				wReceivedBufferLength = USART_RX_DATA_SIZE + wCom2RxBufferEnd - wCom2RxBufferStart;
			}
			break;
		case COM3:
			break;
		default:
			break;
	}

	return wReceivedBufferLength;
}

u16 GetComTxBufferDataLength(COM_TypeDef cComPort)
{
	u16 wTransferBufferLength = 0;

	switch(cComPort)
	{
		case COM1:
			break;
		case COM2:
			if(wCom2TxBufferStart <= wCom2TxBufferEnd)
			{
				wTransferBufferLength =  wCom2TxBufferEnd - wCom2TxBufferStart;
			}
			else
			{
				wTransferBufferLength = USART_TX_DATA_SIZE + wCom2TxBufferEnd - wCom2TxBufferStart;
			}
			break;
		case COM3:
			break;
		default:
			break;
	}

	return wTransferBufferLength;
}


void Com1_LowLevelInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(BF_COM1_TX_GPIO_CLK | BF_COM1_RX_GPIO_CLK, ENABLE);

	/*!< USART1 Periph clock enable */
  	RCC_APB2PeriphClockCmd(BF_COM1_CLK, ENABLE);
	
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = BF_COM1_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BF_COM1_TX_GPIO_PORT, &GPIO_InitStructure);


	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = BF_COM1_RX_PIN;
	GPIO_Init(BF_COM1_TX_GPIO_PORT, &GPIO_InitStructure);

}

void Com2_LowLevelInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable the USART2 Pins Software Remapping */
	
	RCC_APB2PeriphClockCmd(BF_COM2_TX_GPIO_CLK | BF_COM2_RX_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
	/*!< USART2 Periph clock enable */
  	RCC_APB1PeriphClockCmd(BF_COM2_CLK, ENABLE);
	
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = BF_COM2_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BF_COM2_TX_GPIO_PORT, &GPIO_InitStructure);


	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = BF_COM2_RX_PIN;
	GPIO_Init(BF_COM2_TX_GPIO_PORT, &GPIO_InitStructure);

}

void Com1_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	
	Com1_LowLevelInit();
	
	/* BF_SIMU_COM1 default configuration */
	/* BF_SIMU_COM1 configured as follow:
		- BaudRate = 115200 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- Parity NONE
		- Hardware flow control disabled
		- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure and enable the USART */
	//BF_SIMU_COMInit(COM1, &USART_InitStructure);
	USART_Init(USART1, &USART_InitStructure);

	/* Enable the USART Receive interrupt */
	//USART_ITConfig(BF_SIMU_COM1_BASE, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
}

void Com2_Init(void)
{
	USART_InitTypeDef USART_InitStructure;

	Com2_LowLevelInit();
	
	/* BF_SIMU_COM1 default configuration */
	/* BF_SIMU_COM1 configured as follow:
		- BaudRate = 115200 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- Parity NONE
		- Hardware flow control disabled
		- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure and enable the USART */
	USART_Init(USART2, &USART_InitStructure);

	/* Enable the USART Receive & Tx  interrupt */
	//USART_ITConfig(BF_SIMU_COM1_BASE, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART2,  USART_IT_TXE, ENABLE);

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);
}


void BF_ComPortInit(void)
{
	ComVariblesInit();
	Com2_Init();
}




