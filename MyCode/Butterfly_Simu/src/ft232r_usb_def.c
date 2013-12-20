/*  ******************************************************************************
  * @file    ft232r_usb_def.c
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    23-Sep-2013
  * @brief   ft232r basic routines prototypes
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "ft232r_usb_def.h"

const uint16_t wFt232rRegInitValue[] =
{
	0x0040,   /* wIndex = 0x0000 */
	0x0304,   /* wIndex = 0x0001 */
	0x0160,   /* wIndex = 0x0002 */
	0x0000,   /* wIndex = 0x0003 */
	0xA02D,   /* wIndex = 0x0004 */
	0x0800,   /* wIndex = 0x0005 */
	0x0000,   /* wIndex = 0x0006 */
	0x980A,   /* wIndex = 0x0007 */
	0xA220,   /* wIndex = 0x0008 */
	0xC212,   /* wIndex = 0x0009 */
	0x2310,   /* wIndex = 0x000A */
	0x0500,   /* wIndex = 0x000B */
	0x0A03,   /* wIndex = 0x000C */
	0x4600,   /* wIndex = 0x000D */
	0x5400,   /* wIndex = 0x000E */
	0x4400,   /* wIndex = 0x000F */
	0x4900,   /* wIndex = 0x0010 */
	0x2003,   /* wIndex = 0x0011 */
	0x4600,   /* wIndex = 0x0012 */
	0x5400,   /* wIndex = 0x0013 */
	0x3200,   /* wIndex = 0x0014 */
	0x3300,   /* wIndex = 0x0015 */
	0x3200,   /* wIndex = 0x0016 */
	0x5200,   /* wIndex = 0x0017 */
	0x2000,   /* wIndex = 0x0018 */
	0x5500,   /* wIndex = 0x0019 */
	0x5300,   /* wIndex = 0x001A */
	0x4200,   /* wIndex = 0x001B */
	0x2000,   /* wIndex = 0x001C */
	0x5500,   /* wIndex = 0x001D */
	0x4100,   /* wIndex = 0x001E */
	0x5200,   /* wIndex = 0x001F */
	0x5400,   /* wIndex = 0x0020 */
	0x1203,   /* wIndex = 0x0021 */
	0x4100,   /* wIndex = 0x0022 */
	0x3700,   /* wIndex = 0x0023 */
	0x3000,   /* wIndex = 0x0024 */
	0x3200,   /* wIndex = 0x0025 */
	0x4F00,   /* wIndex = 0x0026 */
	0x5700,   /* wIndex = 0x0027 */
	0x5900,   /* wIndex = 0x0028 */
	0x3500,   /* wIndex = 0x0029 */
	0x853E,   /* wIndex = 0x002A */
	0xAB70,   /* wIndex = 0x002B */
	0x0000,   /* wIndex = 0x002C */
	0x0000,   /* wIndex = 0x002D */
	0x0000,   /* wIndex = 0x002E */
	0x0000,   /* wIndex = 0x002F */
	0x0000,   /* wIndex = 0x0030 */
	0x0000,   /* wIndex = 0x0031 */
	0x0000,   /* wIndex = 0x0032 */
	0x0000,   /* wIndex = 0x0033 */
	0x0000,   /* wIndex = 0x0034 */
	0x0000,   /* wIndex = 0x0035 */
	0x0000,   /* wIndex = 0x0036 */
	0x0000,   /* wIndex = 0x0037 */
	0x0000,   /* wIndex = 0x0038 */
	0x0000,   /* wIndex = 0x0039 */
	0x0000,   /* wIndex = 0x003A */
	0x0000,   /* wIndex = 0x003B */
	0x0000,   /* wIndex = 0x003C */
	0x0000,   /* wIndex = 0x003D */
	0x0000,   /* wIndex = 0x003E */
	0x6016,   /* wIndex = 0x003F */
	0x1504,   /* wIndex = 0x0040 */
	0xEAFB,   /* wIndex = 0x0041 */
	0x0000,   /* wIndex = 0x0042 */
	0x853E,   /* wIndex = 0x0043 */
	0xAB70,   /* wIndex = 0x0044 */
	0x0000,   /* wIndex = 0x0045 */
	0x0000,   /* wIndex = 0x0046 */
	0x0000,   /* wIndex = 0x0047 */
	0x0000,   /* wIndex = 0x0048 */
	0x0000,   /* wIndex = 0x0049 */
	0x0000,   /* wIndex = 0x004A */
	0x0000,   /* wIndex = 0x004B */
	0x3741,   /* wIndex = 0x004C */
	0x4957,   /* wIndex = 0x004D */
	0x4330,   /* wIndex = 0x004E */
	0x5053   /* wIndex = 0x004F */
};

uint16_t wFt232rRegArray[0x50];
u8 cIdleData[0x2];

void Ft232rRegisterInitialize(void)
{
	uint16_t i, wTemp, wTemp2;
	uint16_t * wpInitValuePointer;

	wpInitValuePointer = (uint16_t *) wFt232rRegInitValue;

	for(i=0; i<0x50; i++)
	{
		wTemp = * (wpInitValuePointer);
		wTemp2 = wTemp;
		wTemp2 <<= 8;
		wTemp >>= 8;
		wTemp2 |= wTemp;
		wFt232rRegArray[i] = wTemp2;
		wpInitValuePointer ++;
	}
	cIdleData[0] = 0x01;
	cIdleData[1] = 0x60;
}
