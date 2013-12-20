/*
  ******************************************************************************
  * @file    Usb_loopback.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   Usb loopback routines prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_LOOPBACK_H
#define __USB_LOOPBACK_H

typedef enum _USB_BUFF_MODE
{
	BUFF_OVERTURN = 0,
	BUFF_WAITING
}USB_BUFF_MODE;

#define USB_LOOP_BUFFER_SIZE 256

u16 CopyUsbPacketToReceiveLoopBuffer(u8 ucEndpointNumber);
u16 CopySendLoopBufferToUsbPacket(u8 ucEndpointNumber, u16 wLength);
void SetUsbSend(uint8_t ucEndpointNumber, uint16_t wLength);
void SetUsbReceiveEnable(uint8_t ucEndpointNumber);
u16 CopyReceiveBufferToSendBuffer(u16 wLength);

#endif /* __USB_INT_H */
