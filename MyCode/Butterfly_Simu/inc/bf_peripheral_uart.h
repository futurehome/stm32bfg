/*
  ******************************************************************************
  * @file    bf_peripheral_uart.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   uart routines prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BF_PERIPHERAL_UART_H
#define __BF_PERIPHERAL_UART_H

 #include "stm32f10x.h"

#define BF_COM1_IRQHandler                USART1_IRQHandler 
#define Com2_Isr							USART2_IRQHandler
#define COMn                             2
#define USART_RX_DATA_SIZE   	128
#define USART_TX_DATA_SIZE		1024

/* STM32F10x Uart register offset*/
#define USART1_REG_BASE USART1_BASE
#define USART2_REG_BASE USART2_BASE
#define USART3_REG_BASE USART3_BASE

#define	USART_REG_SR	0x00
#define	USART_REG_DR	0x04
#define	USART_REG_BRR	0x08
#define	USART_REG_CR1	0x0C
#define	USART_REG_CR2	0x10
#define	USART_REG_CR3	0x14
#define	USART_REG_GTPR	0x18

#define	USART_REG_CR1_SBK		0x0001
#define	USART_REG_CR1_RWU	0x0002
#define	USART_REG_CR1_RE		0x0004
#define	USART_REG_CR1_TE		0x0008
#define	USART_REG_CR1_IDLEIE	0x0010
#define	USART_REG_CR1_RXNEIE	0x0020
#define	USART_REG_CR1_TCIE	0x0040
#define	USART_REG_CR1_TXEIE	0x0080
#define	USART_REG_CR1_PEIE	0x0100
#define	USART_REG_CR1_PS		0x0200
#define	USART_REG_CR1_PCE		0x0400
#define	USART_REG_CR1_WAKE	0x0800
#define	USART_REG_CR1_M		0x1000
#define	USART_REG_CR1_UE		0x2000



typedef enum 
{
  COM1 = 0,
  COM2 = 1,
  COM3 = 2
} COM_TypeDef;

typedef enum
{
	BINARY = 0,
	TTY = 1,
	COM_SELF_LOOP = 2,
	COM_USB_LOOP = 3
} COM_DATA_MODE;

typedef struct _COM_INT_EVENT
{
	u8	cFlagCom1EventRxDataReceived;
	u8	cFlagCom2EventRxDataReceived;
	u8	cFlagCom3EventRxDataReceived;
} COM_INT_EVENT;

void ComVariblesInit(void);
void Com2_Isr(void);
//void ComIntFlagProcess(void);
u8 cFlagComEvent(void);
void ComEventProcess(void);
u16 ComTransmitData(COM_TypeDef cComPort, u8* cSourcePointer, u16 wTransmitLength);
u16 GetComAvailableRxBufferLength(COM_TypeDef cComPort);
u16 GetComAvailableTxBufferLength(COM_TypeDef cComPort);
u16 GetComRxBufferDataLength(COM_TypeDef cComPort);
u16 GetComTxBufferDataLength(COM_TypeDef cComPort);

void Com1_LowLevelInit(void);
void Com1_Init(void);

void Com2_LowLevelInit(void);
void Com2_Init(void);
void BF_ComPortInit(void);










#endif /* __BF_PERIPHERAL_UART_H */
