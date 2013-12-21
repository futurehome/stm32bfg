/**
  ******************************************************************************
  * @file    usb_core.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    28-August-2012
  * @brief   Standard protocol processing (USB v2.0)
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
//#include "usb_vendor.h"
//#include "ft232r_usb_def.h"
#include "usb_endp.h"
#include "stm32f10x.h"
#include "usb_desc.h"
#include "usb_pwr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ValBit(VAR,Place)    (VAR & (1 << Place))
#define SetBit(VAR,Place)    (VAR |= (1 << Place))
#define ClrBit(VAR,Place)    (VAR &= ((1 << Place) ^ 255))
#define Send0LengthData() { _SetEPTxCount(ENDP0, 0); \
    vSetEPTxStatus(EP_TX_VALID); \
  }

#define vSetEPRxStatus(st) (SaveRState = st)
#define vSetEPTxStatus(st) (SaveTState = st)

#define USB_StatusIn() Send0LengthData()
#define USB_StatusOut() vSetEPRxStatus(EP_RX_VALID)

#define StatusInfo0 StatusInfo.bw.bb1 /* Reverse bb0 & bb1 */
#define StatusInfo1 StatusInfo.bw.bb0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t_uint8_t StatusInfo;
u8	cUsbDeviceStatus[2];
u8	cUsbInterfaceStatus[2];
u8	cUsbEndpointStatus[2];
u8	cUsbCurrentConfiguration;
u8	cUsbCurrentInterface;
u8	cUsbCurrentFeature;
u8	cFlagEp0InStatus;


bool Data_Mul_MaxPacketSize = FALSE;

/* Extern variables =================================*/
extern u16	wEp0InLength;
extern u8	cFlagUsbEp1Send;
extern u8	cFlagUsbEp0Send;
extern u8 *	cpEp0InCurrentPointer;
extern uint16_t wFt232rRegArray[0x50];
extern u8 cIdleData[0x2];
extern const uint8_t cDeviceDescriptor[];
extern const uint8_t cConfigDescriptor[];
extern USB_INT_EVENT UsbEvent;
extern u8 cUsbConfigured;

//extern VENDOR_PROCESS * pVendorProcess;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Standard_GetConfiguration.
* Description    : Return the current configuration variable address.
* Input          : Length - How many bytes are needed.
* Output         : None.
* Return         : Return 1 , if the request is invalid when "Length" is 0.
*                  Return "Buffer" if the "Length" is not 0.
*******************************************************************************/
uint8_t *Standard_GetConfiguration(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength =
      sizeof(pInformation->Current_Configuration);
    return 0;
  }
  pUser_Standard_Requests->User_GetConfiguration();
  return (uint8_t *)&pInformation->Current_Configuration;
}

/*******************************************************************************
* Function Name  : Standard_SetConfiguration.
* Description    : This routine is called to set the configuration value
*                  Then each class should configure device itself.
* Input          : None.
* Output         : None.
* Return         : Return USB_SUCCESS, if the request is performed.
*                  Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetConfiguration(void)
{

  if ((pInformation->USBwValue0 <=
      Device_Table.Total_Configuration) && (pInformation->USBwValue1 == 0)
      && (pInformation->USBwIndex == 0)) /*call Back usb spec 2.0*/
  {
    pInformation->Current_Configuration = pInformation->USBwValue0;
    pUser_Standard_Requests->User_SetConfiguration();

    return USB_SUCCESS;
  }
  else
  {
    return USB_UNSUPPORT;
  }
}

/*******************************************************************************
* Function Name  : Standard_GetInterface.
* Description    : Return the Alternate Setting of the current interface.
* Input          : Length - How many bytes are needed.
* Output         : None.
* Return         : Return 0, if the request is invalid when "Length" is 0.
*                  Return "Buffer" if the "Length" is not 0.
*******************************************************************************/
uint8_t *Standard_GetInterface(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength =
      sizeof(pInformation->Current_AlternateSetting);
    return 0;
  }
  pUser_Standard_Requests->User_GetInterface();
  return (uint8_t *)&pInformation->Current_AlternateSetting;
}

/*******************************************************************************
* Function Name  : Standard_SetInterface.
* Description    : This routine is called to set the interface.
*                  Then each class should configure the interface them self.
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetInterface(void)
{
  RESULT Re;
  /*Test if the specified Interface and Alternate Setting are supported by
    the application Firmware*/
  Re = (*pProperty->Class_Get_Interface_Setting)(pInformation->USBwIndex0, pInformation->USBwValue0);

  if (pInformation->Current_Configuration != 0)
  {
    if ((Re != USB_SUCCESS) || (pInformation->USBwIndex1 != 0)
        || (pInformation->USBwValue1 != 0))
    {
      return  USB_UNSUPPORT;
    }
    else if (Re == USB_SUCCESS)
    {
      pUser_Standard_Requests->User_SetInterface();
      pInformation->Current_Interface = pInformation->USBwIndex0;
      pInformation->Current_AlternateSetting = pInformation->USBwValue0;
      return USB_SUCCESS;
    }

  }

  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Standard_GetStatus.
* Description    : Copy the device request data to "StatusInfo buffer".
* Input          : - Length - How many bytes are needed.
* Output         : None.
* Return         : Return 0, if the request is at end of data block,
*                  or is invalid when "Length" is 0.
*******************************************************************************/
uint8_t *Standard_GetStatus(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = 2;
    return 0;
  }

  /* Reset Status Information */
  StatusInfo.w = 0;

  if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))
  {
    /*Get Device Status */
    uint8_t Feature = pInformation->Current_Feature;

    /* Remote Wakeup enabled */
    if (ValBit(Feature, 5))
    {
      SetBit(StatusInfo0, 1);
    }
    else
    {
      ClrBit(StatusInfo0, 1);
    }      

    /* Bus-powered */
    if (ValBit(Feature, 6))
    {
      SetBit(StatusInfo0, 0);
    }
    else /* Self-powered */
    {
      ClrBit(StatusInfo0, 0);
    }
  }
  /*Interface Status*/
  else if (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
  {
    return (uint8_t *)&StatusInfo;
  }
  /*Get EndPoint Status*/
  else if (Type_Recipient == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))
  {
    uint8_t Related_Endpoint;
    uint8_t wIndex0 = pInformation->USBwIndex0;

    Related_Endpoint = (wIndex0 & 0x0f);
    if (ValBit(wIndex0, 7))
    {
      /* IN endpoint */
      if (_GetTxStallStatus(Related_Endpoint))
      {
        SetBit(StatusInfo0, 0); /* IN Endpoint stalled */
      }
    }
    else
    {
      /* OUT endpoint */
      if (_GetRxStallStatus(Related_Endpoint))
      {
        SetBit(StatusInfo0, 0); /* OUT Endpoint stalled */
      }
    }

  }
  else
  {
    return NULL;
  }
  pUser_Standard_Requests->User_GetStatus();
  return (uint8_t *)&StatusInfo;
}

/*******************************************************************************
* Function Name  : Standard_ClearFeature.
* Description    : Clear or disable a specific feature.
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_ClearFeature(void)
{
  uint32_t     Type_Rec = Type_Recipient;
  uint32_t     Status;


  if (Type_Rec == (STANDARD_REQUEST | DEVICE_RECIPIENT))
  {/*Device Clear Feature*/
    ClrBit(pInformation->Current_Feature, 5);
    return USB_SUCCESS;
  }
  else if (Type_Rec == (STANDARD_REQUEST | ENDPOINT_RECIPIENT))
  {/*EndPoint Clear Feature*/
    DEVICE* pDev;
    uint32_t Related_Endpoint;
    uint32_t wIndex0;
    uint32_t rEP;

    if ((pInformation->USBwValue != ENDPOINT_STALL)
        || (pInformation->USBwIndex1 != 0))
    {
      return USB_UNSUPPORT;
    }

    pDev = &Device_Table;
    wIndex0 = pInformation->USBwIndex0;
    rEP = wIndex0 & ~0x80;
    Related_Endpoint = ENDP0 + rEP;

    if (ValBit(pInformation->USBwIndex0, 7))
    {
      /*Get Status of endpoint & stall the request if the related_ENdpoint
      is Disabled*/
      Status = _GetEPTxStatus(Related_Endpoint);
    }
    else
    {
      Status = _GetEPRxStatus(Related_Endpoint);
    }

    if ((rEP >= pDev->Total_Endpoint) || (Status == 0)
        || (pInformation->Current_Configuration == 0))
    {
      return USB_UNSUPPORT;
    }


    if (wIndex0 & 0x80)
    {
      /* IN endpoint */
      if (_GetTxStallStatus(Related_Endpoint ))
      {
        ClearDTOG_TX(Related_Endpoint);
        SetEPTxStatus(Related_Endpoint, EP_TX_VALID);
      }
    }
    else
    {
      /* OUT endpoint */
      if (_GetRxStallStatus(Related_Endpoint))
      {
        if (Related_Endpoint == ENDP0)
        {
          /* After clear the STALL, enable the default endpoint receiver */
          SetEPRxCount(Related_Endpoint, Device_Property.MaxPacketSize);
          _SetEPRxStatus(Related_Endpoint, EP_RX_VALID);
        }
        else
        {
          ClearDTOG_RX(Related_Endpoint);
          _SetEPRxStatus(Related_Endpoint, EP_RX_VALID);
        }
      }
    }
    pUser_Standard_Requests->User_ClearFeature();
    return USB_SUCCESS;
  }

  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Standard_SetEndPointFeature
* Description    : Set or enable a specific feature of EndPoint
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetEndPointFeature(void)
{
  uint32_t    wIndex0;
  uint32_t    Related_Endpoint;
  uint32_t    rEP;
  uint32_t    Status;

  wIndex0 = pInformation->USBwIndex0;
  rEP = wIndex0 & ~0x80;
  Related_Endpoint = ENDP0 + rEP;

  if (ValBit(pInformation->USBwIndex0, 7))
  {
    /* get Status of endpoint & stall the request if the related_ENdpoint
    is Disabled*/
    Status = _GetEPTxStatus(Related_Endpoint);
  }
  else
  {
    Status = _GetEPRxStatus(Related_Endpoint);
  }

  if (Related_Endpoint >= Device_Table.Total_Endpoint
      || pInformation->USBwValue != 0 || Status == 0
      || pInformation->Current_Configuration == 0)
  {
    return USB_UNSUPPORT;
  }
  else
  {
    if (wIndex0 & 0x80)
    {
      /* IN endpoint */
      _SetEPTxStatus(Related_Endpoint, EP_TX_STALL);
    }

    else
    {
      /* OUT endpoint */
      _SetEPRxStatus(Related_Endpoint, EP_RX_STALL);
    }
  }
  pUser_Standard_Requests->User_SetEndPointFeature();
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Standard_SetDeviceFeature.
* Description    : Set or enable a specific feature of Device.
* Input          : None.
* Output         : None.
* Return         : - Return USB_SUCCESS, if the request is performed.
*                  - Return USB_UNSUPPORT, if the request is invalid.
*******************************************************************************/
RESULT Standard_SetDeviceFeature(void)
{
  SetBit(pInformation->Current_Feature, 5);
  pUser_Standard_Requests->User_SetDeviceFeature();
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Standard_GetDescriptorData.
* Description    : Standard_GetDescriptorData is used for descriptors transfer.
*                : This routine is used for the descriptors resident in Flash
*                  or RAM
*                  pDesc can be in either Flash or RAM
*                  The purpose of this routine is to have a versatile way to
*                  response descriptors request. It allows user to generate
*                  certain descriptors with software or read descriptors from
*                  external storage part by part.
* Input          : - Length - Length of the data in this transfer.
*                  - pDesc - A pointer points to descriptor struct.
*                  The structure gives the initial address of the descriptor and
*                  its original size.
* Output         : None.
* Return         : Address of a part of the descriptor pointed by the Usb_
*                  wOffset The buffer pointed by this address contains at least
*                  Length bytes.
*******************************************************************************/
uint8_t *Standard_GetDescriptorData(uint16_t Length, ONE_DESCRIPTOR *pDesc)
{
  uint32_t  wOffset;

  wOffset = pInformation->Ctrl_Info.Usb_wOffset;
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = pDesc->Descriptor_Size - wOffset;
    return 0;
  }

  return pDesc->Descriptor + wOffset;
}

/*******************************************************************************
* Function Name  : Data_Setup0.
* Description    : Proceed the processing of setup request with data stage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Data_Setup0(void)
{
	u8 Request_No;
	u8 cValue1;
	u8 cValue0;
	u8 cEndpointIndex;
	//u16 wVendorRegIndex;

	cValue1 = pInformation->USBwValue1;
	cValue0 = pInformation->USBwValue0;
	cEndpointIndex = pInformation->USBwIndex0 & 0x0F;
	Request_No = pInformation->USBbRequest;
	cFlagUsbEp0Send = USB_EP0_IN_SEND;

	//SetEPTxStatus(ENDP1, EP_TX_NAK);
	//cFlagUsbEp1Send = 0;
	if(Type_Recipient & 0x40)		//test bit6 if it is a vendor command
	{
		if(pInformation->USBwLength != 0) /* need prepare IN data */
		{
			switch (Request_No)
			{
				case FT232R_VENDOR_REQ_GET_90:
					//cEndpointIndex = pInformation ->USBwIndex0;
					//cpEp0InCurrentPointer = (u8*)(wFt232rRegArray + cEndpointIndex);
					cIdleData[0] = 0xFF;
					cIdleData[1] = 0xFF;
					cpEp0InCurrentPointer = (u8*) cIdleData;
					wEp0InLength = 2;
					break;
				case FT232R_VENDOR_REQ_GET_05:
					cIdleData[0] = 0x01;
					cIdleData[1] = 0x60;
					cpEp0InCurrentPointer = (u8*) cIdleData;
					wEp0InLength = 2;
					break;
				default:
					SetEp0InStall();
			}
		}
		else
		{
			switch (Request_No)
			{
				case FT232R_VENDOR_REQ_SET_00:
					wEp0InLength = 0;
					break;
				case FT232R_VENDOR_REQ_SET_01:
					wEp0InLength = 0;
					break;
				case FT232R_VENDOR_REQ_SET_02:
					wEp0InLength = 0;
					break;
				case FT232R_VENDOR_REQ_SET_03:
					wEp0InLength = 0;
					break;
				case FT232R_VENDOR_REQ_SET_04:
					wEp0InLength = 0;
					break;
				case FT232R_VENDOR_REQ_SET_06:
					wEp0InLength = 0;
					break;
				case FT232R_VENDOR_REQ_SET_09:
					wEp0InLength = 0;
					bDeviceState = CONFIGURED;
					break;
				default:
					SetEp0InStall();
			}
		}
	}
	else		//USB Standard command
	{
	
		switch (Request_No)
		{
			case GET_DESCRIPTOR:			/*GET DESCRIPTOR*/
				if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))
				{
					switch (cValue1)
					{
						case DEVICE_DESCRIPTOR:			//Device
							cpEp0InCurrentPointer = (u8 *)cDeviceDescriptor;		
							wEp0InLength = DEVICE_DESCRIPTOR_SIZE;
							break;
						case CONFIG_DESCRIPTOR:		//configuration
							cpEp0InCurrentPointer = (u8 *)cConfigDescriptor;
							wEp0InLength = CONFIG_DESCRIPTOR_SIZE;
							break;
						case STRING_DESCRIPTOR:		//string
							switch(cValue0)
							{
								case 0:
									cpEp0InCurrentPointer = (u8 *) cStringLangID;
									wEp0InLength  = LANGID_STRING_SIZE;
									break;
								case 1:
									cpEp0InCurrentPointer = (u8 *) cStringVendor;
									wEp0InLength  = VENDOR_STRING_SIZE;
									break;
								case 2:
									cpEp0InCurrentPointer = (u8 *) cStringProduct;
									wEp0InLength  = PRODUCT_STRING_SIZE;
									break;
								case 3:
									cpEp0InCurrentPointer = (u8 *) cStringSerial;
									wEp0InLength  = SERIAL_STRING_SIZE;
									break;
								default:
									SetEp0InStall();
							}
							break;
						default:
							SetEp0InStall();
					}
				}
				else
				{
					SetEp0InStall();
				}
				break;
			case  GET_STATUS:			/*GET STATUS*/
				if( (pInformation->USBwValue == 0)&& (pInformation->USBwLength == 0x0002)
						&& (pInformation->USBwIndex1 == 0))
				{
					switch (Type_Recipient)
					{
						case (STANDARD_REQUEST | DEVICE_RECIPIENT):  		/* GET STATUS for Device*/
							if(pInformation->USBwIndex == 0)
							{
								cpEp0InCurrentPointer = (u8 *)cUsbDeviceStatus;
								wEp0InLength = 2;
							}
							break;
						case (STANDARD_REQUEST | INTERFACE_RECIPIENT):		/* GET STATUS for Interface*/
							cpEp0InCurrentPointer = (u8 *)cUsbInterfaceStatus;
							wEp0InLength = 2;
							break;
						case (STANDARD_REQUEST | ENDPOINT_RECIPIENT):		/* GET STATUS for EndPoint*/
							cEndpointIndex = (pInformation->USBwIndex0 & 0x0f);
							if(cEndpointIndex <= 2)
							{
								cUsbEndpointStatus[0] = 0x00;
								if ((pInformation->USBwIndex0) & 0x80)			//test endpoint direction, 1: IN, 0: OUT;
								{
									if( _GetEPTxStatus(cEndpointIndex))
									{
										cUsbEndpointStatus[0] = 0x01;			//bit0=1, means endpoint halt;
									}
								}
								else
								{
									if(_GetEPRxStatus(cEndpointIndex))
									{
										cUsbEndpointStatus[0] = 0x01;			//if endpoint halt/stalled, set bit0=1
									}
								}
								cpEp0InCurrentPointer = (u8 *)cUsbEndpointStatus;
								wEp0InLength = 2;
							}
							break;
						default:
							SetEp0InStall();
					}
				}
				else
				{
					SetEp0InStall();
				}
				break;
			case GET_CONFIGURATION:				/*GET CONFIGURATION*/
				if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))
				{
					if(((pInformation->USBwValue) == 0) &&((pInformation->USBwIndex) == 0) && ((pInformation->USBwLength)==1))
					{
						wEp0InLength = 1;
						cpEp0InCurrentPointer = &cUsbCurrentConfiguration;
					}
					else
					{
						SetEp0InStall();
					}
				}
				else
				{
					SetEp0InStall();
				}
				break;
			case GET_INTERFACE:			/*GET INTERFACE*/
				if ((Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
					&& (pInformation->Current_Configuration != 0) && (pInformation->USBwValue == 0)
					&& (pInformation->USBwIndex1 == 0) && (pInformation->USBwLength == 0x0001))
				{
					cpEp0InCurrentPointer = &cUsbCurrentInterface;
					wEp0InLength = 1;
				}
				else
				{
					SetEp0InStall();
				}
				break;
			case SET_CONFIGURATION:		/* SET_CONFIGURATION*/
				cUsbCurrentConfiguration = cValue0;
				wEp0InLength = 0;
				cUsbConfigured = TRUE;
				break;
			case  SET_ADDRESS:			/*SET ADDRESS*/
				if ((pInformation->USBwValue0 > 127) || (pInformation->USBwValue1 != 0)
					|| (pInformation->USBwIndex != 0)
					|| (pInformation->Current_Configuration != 0))
				{
					SetEp0InStall();
				}
				else
				{
					wEp0InLength = 0;
				}
				break;					//if address<=127, do nothing, till the following IN token it will be valid and fill register
			case SET_FEATURE:			
				if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))		/*SET FEATURE for Device*/
				{
					if ((cValue0 == DEVICE_REMOTE_WAKEUP) && (pInformation->USBwIndex == 0))
					{
						cUsbCurrentFeature |= 0x20;
						wEp0InLength = 0;
					}
					else
					{
						SetEp0InStall();
					}
				}
				else if(Type_Recipient == (STANDARD_REQUEST | ENDPOINT_RECIPIENT)) 	/* SET FEATURE for EndPoint*/
				{
					//if(Standard_SetEndPointFeature())		//if return !=0, endpoint is disabled
					//{
					//	SetEp0InStall();
					//}
					//else									//set the endpoint halt
					//{
						wEp0InLength = 0;
						
					//}		
				}
				else
				{
					SetEp0InStall();
				}
				break;
			case CLEAR_FEATURE:			
				if (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT))		/*Clear FEATURE for Device */
				{
					if (cValue0 == DEVICE_REMOTE_WAKEUP	&& pInformation->USBwIndex == 0)
					{
						cUsbCurrentFeature &= 0x20;
						wEp0InLength = 0;
					}
					else
					{
						SetEp0InStall();
					}
				}
				else if(Type_Recipient == (STANDARD_REQUEST | ENDPOINT_RECIPIENT)) 	/* Clear FEATURE for EndPoint*/
				{
					//if(Standard_ClearFeature())			//if return !=0, endpoint is disabled
					//{
					//	SetEp0InStall();
					//}
					//else								//set the endpoint active
					//{
						wEp0InLength = 0;

					//}
				}
				else
				{
					SetEp0InStall();
				}
				break;
			default:
				SetEp0InStall();
		}
	}
	if(wEp0InLength >= pInformation->USBwLength)
	{
		wEp0InLength = pInformation->USBwLength;
		cFlagEp0InStatus = 0;
	}
	else if( pInformation->USBwLength > 200) 
	{
		cFlagEp0InStatus = 1;
		//if( (Request_No == GET_DESCRIPTOR) && (cValue1 == STRING_DESCRIPTOR) && (Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT)))
		//{
			//cFlagEp0InStatus = 0;
		//}
	}
	else
	{
		cFlagEp0InStatus = 0;
	}
	if(cFlagUsbEp0Send == USB_EP0_IN_SEND)
	{
		Ep0InProcess();
	}
	else
	{
		SetEPTxStatus(ENDP0, EP_TX_STALL);
	}
}

/*******************************************************************************
* Function Name  : Setup0_Process
* Description    : Get the device request data and dispatch to individual process.
* Input          : None.
* Output         : None.
* Return         : Post0_Process.
*******************************************************************************/
void Setup0_Process(void)
{

	union
	{
		uint8_t* b;
		uint16_t* w;
	} pBuf;
	uint16_t offset = 1;
  
	pBuf.b = PMAAddr + (uint8_t *)(_GetEPRxAddr(ENDP0) * 2); /* *2 for 32 bits addr */

	if (pInformation->ControlState != PAUSE)
	{
		pInformation->USBbmRequestType = *pBuf.b++; /* bmRequestType */
		pInformation->USBbRequest = *pBuf.b++; /* bRequest */
		pBuf.w += offset;  /* word not accessed because of 32 bits addressing */
		pInformation->USBwValue = ByteSwap(*pBuf.w++); /* wValue */
		pBuf.w += offset;  /* word not accessed because of 32 bits addressing */
		pInformation->USBwIndex  = ByteSwap(*pBuf.w++); /* wIndex */
		pBuf.w += offset;  /* word not accessed because of 32 bits addressing */
		pInformation->USBwLength = *pBuf.w; /* wLength */
	}

	
	UsbEvent.cFlagUsbEventEp0Setup = 1;
    
}

/*******************************************************************************
* Function Name  : In0_Process
* Description    : Process the IN token on all default endpoint.
* Input          : None.
* Output         : None.
* Return         : Post0_Process.
*******************************************************************************/
void In0_Process(void)
{
	UsbEvent.cFlagUsbEventEp0In = 1;

	if ((pInformation->USBbRequest == SET_ADDRESS) &&
		(Type_Recipient == (STANDARD_REQUEST | DEVICE_RECIPIENT)))
	{
		SetDeviceAddress(pInformation->USBwValue0);
		wEp0InLength = 0;
	}
}

/*******************************************************************************
* Function Name  : Out0_Process
* Description    : Process the OUT token on all default endpoint.
* Input          : None.
* Output         : None.
* Return         : Post0_Process.
*******************************************************************************/
void Out0_Process(void)
{
	UsbEvent.cFlagUsbEventEp0Out = 1;
}

/*******************************************************************************
* Function Name  : Post0_Process
* Description    : Stall the Endpoint 0 in case of error.
* Input          : None.
* Output         : None.
* Return         : - 0 if the control State is in PAUSE
*                  - 1 if not.
*******************************************************************************/
uint8_t Post0_Process(void)
{
   
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);

  if (pInformation->ControlState == STALLED)
  {
    vSetEPRxStatus(EP_RX_STALL);
    vSetEPTxStatus(EP_TX_STALL);
  }

  return (pInformation->ControlState == PAUSE);
}

/*******************************************************************************
* Function Name  : SetDeviceAddress.
* Description    : Set the device and all the used Endpoints addresses.
* Input          : - Val: device address.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SetDeviceAddress(uint8_t Val)
{
  uint32_t i;
  uint32_t nEP = Device_Table.Total_Endpoint;

  /* set address in every used endpoint */
  for (i = 0; i < nEP; i++)
  {
    _SetEPAddress((uint8_t)i, (uint8_t)i);
  } /* for */
  _SetDADDR(Val | DADDR_EF); /* set device address and enable function */ 
}

/*******************************************************************************
* Function Name  : NOP_Process
* Description    : No operation function.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void NOP_Process(void)
{
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
