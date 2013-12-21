/*
  ******************************************************************************
  * @file    usb_vendor.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    23-Sep-2013
  * @brief   Usb vendor routine prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_VENDOR_H
#define __USB_VENDOR_H





u8 * VendorGetRegisterValue(u16 wIndex);
u8 * VendorGetStatus(u16 wLength);
RESULT VendorSetCommand00(void);
RESULT VendorSetCommand01(void);
RESULT VendorSetCommand02(void);
RESULT VendorSetCommand03(void);
RESULT VendorSetCommand04(void);
RESULT VendorSetCommand06(void);
RESULT VendorSetCommand09(void);

#endif /* __USB_VENDOR_H */

