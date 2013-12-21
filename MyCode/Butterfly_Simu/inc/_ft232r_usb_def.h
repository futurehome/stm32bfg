/*
  ******************************************************************************
  * @file    ft232r_usb_def.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    23-Sep-2013
  * @brief   FT232R Usb Control Pipe prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FT232R_USB_DEF_H
#define __FT232R_USB_DEF_H

#define FT232R_VENDOR_REQ_GET_90 0x90
#define FT232R_VENDOR_REQ_GET_05 0x05

#define FT232R_VENDOR_REQ_SET_00 0x00
#define FT232R_VENDOR_REQ_SET_01 0x01
#define FT232R_VENDOR_REQ_SET_02 0x02
#define FT232R_VENDOR_REQ_SET_03 0x03
#define FT232R_VENDOR_REQ_SET_04 0x04
#define FT232R_VENDOR_REQ_SET_06 0x06
#define FT232R_VENDOR_REQ_SET_09 0x09


void Ft232rRegisterInitialize(void);

#endif /* __FT232R_USB_DEF_H */
