/**
  ******************************************************************************
  * @file    usb_desc.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Descriptors for Virtual Com Port Demo
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"

/* USB Standard Device Descriptor */
const uint8_t cDeviceDescriptor[] =
  {
    0x12,   /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,     /* bDescriptorType */
    0x00,
    0x02,   /* bcdUSB = 2.00 */
    0x00,   /* bDeviceClass: CDC */ //convert to FT232
    0x00,   /* bDeviceSubClass */
    0x00,   /* bDeviceProtocol */
    MAX_USB_CTRL_PACKET_SIZE,   /* bMaxPacketSize0 */
    0x03,
    0x04,   /* idVendor = 0x0483 */  //FT232
    0x14,
    0x60,   /* idProduct = 0x7540 */ //..FT232
    0x00,
    0x08,   /* bcdDevice = 2.00 */   //like usb blaster
    1,              /* Index of string descriptor describing manufacturer */
    2,              /* Index of string descriptor describing product */
    3,              /* Index of string descriptor describing the device's serial number */
    0x01    /* bNumConfigurations */
  };

const uint8_t cConfigDescriptor[] =
  {
    /*Configuration Descriptor*/
    0x09,   /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */
    CONFIG_DESCRIPTOR_SIZE,       /* wTotalLength:no of returned bytes */
    0x00,
    0x01,   /* bNumInterfaces: 2 interface */    //ft232
    0x01,   /* bConfigurationValue: Configuration value */
    0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
    0xC0,   /* bmAttributes: self powered */ //bus power bit7=1; bit6,self power;bit5 remote wakeup
    0x2D,   /* MaxPower 0 mA */  //90mA
    /*Interface Descriptor*/
    0x09,   /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints: One endpoints used */
    0xFF,   /* bInterfaceClass: Communication Interface Class */
    0xFF,   /* bInterfaceSubClass: Abstract Control Model */
    0xFF,   /* bInterfaceProtocol: Common AT commands */
    0x02,   /* iInterface: */ //index of string of this interface
    
    /*Endpoint 2 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x81,   /* bEndpointAddress: (IN1) */  //bit7 IN; bit3-0, ep number
    0x02,   /* bmAttributes: bulk */
    MAX_USB_DATA_PACKET_SIZE,      /* wMaxPacketSize: */
    0x00,
    0x00,   /* bInterval: */
    
    /*Endpoint 3 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x02,   /* bEndpointAddress: (OUT2) */
    0x02,   /* bmAttributes: Bulk */
    MAX_USB_DATA_PACKET_SIZE,             /* wMaxPacketSize: */
    0x00,
    0x00,   /* bInterval: ignore for Bulk transfer */
    
  };

/* USB String Descriptors */
const uint8_t cStringLangID[LANGID_STRING_SIZE] =
  {
    LANGID_STRING_SIZE,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04 /* LangID = 0x0409: U.S. English */
  };

const uint8_t cStringVendor[VENDOR_STRING_SIZE] =
  {
    VENDOR_STRING_SIZE,     /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,             /* bDescriptorType*/
    /* Manufacturer: "STMicroelectronics" */
    'B', 0, 'F', 0, '-', 0, 'C', 0, 'h', 0, 'i', 0, 'n', 0, 'a', 0,
    ' ', 0, 'C', 0, 'h', 0, 'e', 0, 'n', 0, 'g', 0, 'd', 0, 'u', 0,
    'W', 0, 'G', 0, 'J', 0
  };

const uint8_t cStringProduct[PRODUCT_STRING_SIZE] =
  {
    PRODUCT_STRING_SIZE,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    /* Product name: "STM32 Virtual COM Port" */
    'B', 0, 'i', 0, 't', 0, 'F', 0, 'O', 0, 'R', 0, 'C', 0, 'E', 0,
    ' ', 0, 'S', 0, 'H', 0, 'A', 0, '2', 0, '5', 0, '6', 0, ' ', 0,
    'S', 0, 'C', 0
  };

const uint8_t cStringSerial[SERIAL_STRING_SIZE] =
  {
    SERIAL_STRING_SIZE,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0
  };

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
