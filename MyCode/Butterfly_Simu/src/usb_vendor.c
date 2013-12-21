/*  ******************************************************************************
  * @file    usb_vendor.c
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    23-Sep-2013
  * @brief   Usb vendor routines prototypes
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include "stm32f10x.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_core.h"

#include "usb_vendor.h"



#if defined FT232R_SIMU
#include "ft232r_usb_def.h"
#endif

extern __IO uint16_t SaveRState;
extern __IO uint16_t SaveTState;
extern uint16_t  wFt232rRegArray[0x50];
extern u8 cIdleData[0x2];

u32 dwVendorUsbConfiged;


u8 * VendorGetRegisterValue(u16 wLength)
{
	u8 * cpVendorRegisterValue;
	uint16_t  wOffset;
	u16 wTemp;

	wTemp = pInformation->USBwIndex;
	wOffset = (wTemp>>8) | (wTemp<<8);

	if (wLength == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = 2;
		return 0;
	}

	cpVendorRegisterValue =  (u8 *)(wFt232rRegArray + wOffset);

	return cpVendorRegisterValue;
}

u8 * VendorGetStatus(u16 wLength)
{
	u8* cpVendorStatus;

	if (wLength == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = 2;
		return 0;
	}

	cpVendorStatus = cIdleData;

	return cpVendorStatus;
}

RESULT VendorSetCommand00(void)
{
	return USB_SUCCESS;
}

RESULT VendorSetCommand01(void)
{
	return USB_SUCCESS;
}

RESULT VendorSetCommand02(void)
{
	return USB_SUCCESS;
}

RESULT VendorSetCommand03(void)
{
	return USB_SUCCESS;
}

RESULT VendorSetCommand04(void)
{
	return USB_SUCCESS;
}

RESULT VendorSetCommand06(void)
{
	return USB_SUCCESS;
}

RESULT VendorSetCommand09(void)
{
	dwVendorUsbConfiged = TRUE;
	return USB_SUCCESS;
}


