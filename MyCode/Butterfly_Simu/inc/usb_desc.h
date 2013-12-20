/**
  ******************************************************************************
  * @file    usb_desc.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Descriptor Header for Virtual COM Port Device
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define MAX_USB_DATA_PACKET_SIZE              64
#define MAX_USB_CTRL_PACKET_SIZE			64
//#define VIRTUAL_COM_PORT_INT_SIZE               8

#define DEVICE_DESCRIPTOR_SIZE        18
#define CONFIG_DESCRIPTOR_SIZE        32
#define LANGID_STRING_SIZE      4
#define VENDOR_STRING_SIZE      40
#define PRODUCT_STRING_SIZE     38
#define SERIAL_STRING_SIZE      18

#define STANDARD_ENDPOINT_DESC_SIZE             0x09

/* Exported functions ------------------------------------------------------- */
extern const uint8_t cDeviceDescriptor[DEVICE_DESCRIPTOR_SIZE];
extern const uint8_t cConfigDescriptor[CONFIG_DESCRIPTOR_SIZE];

extern const uint8_t cStringLangID[LANGID_STRING_SIZE];
extern const uint8_t cStringVendor[VENDOR_STRING_SIZE];
extern const uint8_t cStringProduct[PRODUCT_STRING_SIZE];
extern const uint8_t cStringSerial[SERIAL_STRING_SIZE];

#endif /* __USB_DESC_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
