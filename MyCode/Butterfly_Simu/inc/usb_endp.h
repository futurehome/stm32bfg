/*
  ******************************************************************************
  * @file    usb_endp.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   Uusb endpoint routines prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_ENDP_H
#define __USB_ENDP_H

typedef enum _USB_BUFF_MODE
{
	BUFF_OVERWRITE = 0,
	BUFF_WAITING
}USB_BUFF_MODE;

typedef struct _USB_INT_EVENT
{
	u8	cFlagUsbEventEp2Out;
	u8	cFlagUsbEventEp0Setup;
	u8	cFlagUsbEventEp0In;
	u8	cFlagUsbEventEp0Out;
	u8	cFlagUsbEventWakeup;
} USB_INT_EVENT;

typedef struct _USB_EP_PROCESS
{
	u8	cEp0Setup;
	u8	cEp0In;
	u8	cEp0Out;
	u8	cEp1In;
	u8	cEp2Out;
} USB_EP_PROCESS;

#define USB_LOOP_BUFFER_SIZE 1536
#define USB_IDLE_INTERVAL 15 			// unit mS, using SOF to count

#define	ENDP0_MAX_PACKET_SIZE	0x40
#define	USB_EP0_SETUP 	0x01
#define	USB_EP0_IN		0x02
#define	USB_EP0_OUT	0x04
#define	USB_EP1_IN		0x08
#define	USB_EP2_OUT	0x10

#define	USB_EP0_IN_NAK	0
#define	USB_EP0_IN_SEND	1
#define	USB_EP0_IN_STALL	2

#define USB_DATA_RECEIVED	0x01
#define USB_TX_BUFFER_FULL		0x02
#define	USB_IS_SUSPENDED		0x04
#define	USB_IS_CONFIGURED	0x08

u16 CopyUsbPacketToReceiveLoopBuffer(u8 ucEndpointNumber);
u16 CopySendLoopBufferToUsbPacket(u8 ucEndpointNumber, u16 wLength);
void SetUsbSend(uint8_t ucEndpointNumber, uint16_t wLength);
void SetUsbReceiveEnable(uint8_t ucEndpointNumber);
void SetUsbReceivedStall(u8 ucEndpointNumber);
void UsbEndpointInit(void);
void Ep0InProcess(void);
void Ep0OutProcess(void);
void Ep0SetupProcess(void);
void SetEp0InStall(void);
void UsbEventProcess(void);
void UsbIntFlagProcess(void);
u8 cFlagUsbEvent(void);
u16 GetUsbTxBufferDataLength(void);
u16 GetUsbTxBufferAvailableLength(void);
u16 GetUsbRxBufferDataLength(void);
u16 GetUsbRxBufferAvailableLength(void);
void Ep1FillKeepAliveFlag(void);
void Ep1FillUsbPacket(u16 wFillLength);







#endif /* __USB_ENDP_H */
