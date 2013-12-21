/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Virtual Com Port Demo main file
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
//#include "hw_config.h"
#include "bf_simu_platform.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
//#include "usb_vendor.h"
//#include "ft232r_usb_def.h"
#include "usb_endp.h"
#include "bf_general.h"
#include "bf_peripheral_uart.h"
#include <stdio.h>
#include "bf_simu_peripheral.h"
#include "bf_peripheral_lm75.h"
#include "bf_peripheral_adc.h"
#include "bf_peripheral_power.h"
#include "bf_peripheral_timer.h"
#include "Main_BitforceSC.h"
#include "bf_peripheral_spi.h"
#include "HighLevel_Operations.h"
#include "USBProtocol_Module.h"
#include "HostInteractionProtocols.h"
#include "bf_peripheral_timer.h"
#include "jobpipe_module.h"





/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/

extern u8 cFlagUsbDataReceived;
extern u16 	wSendLoopBufferStart;
extern u16 	wSendLoopBufferEnd;
extern u8		cFlagEp0Out;
extern USB_INT_EVENT UsbEvent;
extern u8		cFlagUsbEp1Send;
extern u8	cFlagCom2DataReceived;

extern int XLINK_ARE_WE_MASTER;

extern u32	dwSofCount;
extern char sz_cmd[1024];
extern int global_vals[6];
extern unsigned int DEBUG_LastXLINKTransTook;
extern BF_USB_PROTOCOL_OUT_PIPE_DATA_BELONGS_Def UsbOutDataBelongs;
extern u8 cUsbConfigured;
extern SOFT_TIMER SoftTimer0;
extern u8 cUsbReturnJobIsUnfinished;


uint32_t dwTotalSendBytes;
u8	cFlagHostCommandDataReceived;
u8	cFlagUartCommandDataReceived;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
	//u16 wTemp;
	//char	cTempString[256];
	//u32 dwTemp;
	//u32 i;
	u32 i_count = 0;
	//char bTimeoutDetectedOnXLINK;
	//char bDeviceNotRespondedOnXLINK;
	//unsigned int bEOSDetected;
	//unsigned int iExpectedPacketLength;
	//unsigned int bInterceptingChainForwardReq;
	//unsigned char bSingleStageJobIssueCommand;
	unsigned int intercepted_command_length = 0;
	u8 cTempString[] = {'T', 'E', 'S', 'T', '\n'};
	



	
	GlobalVariableInitialize();
	Set_System();
	Set_USBClock();
	BF_SystemInterruptsConfig();	
	
	
	
	BF_GpioInit();
	BF_PowerInit();
	BF_ComPortInit();
	ComTransmitData(COM2, cTempString, 5);
	BF_ADC_Init();
	BF_ADC_Start();
	BF_Timer1_Init();
	BF_Timer2_Init();
	BF_Timer3_Init();
	//LM75_Init();
	//StartLm75();
	BF_SPI_Init();
	//dwTemp = BF_SpiSimu_FlashReadID();
	AVR_main();
	ClearUsbRxBuffer();
	BF_CommandPrepare();
	UsbEndpointInit();
	USB_Init();
  
	while (1)
	{
		//if(cFlagComEvent())
		//{
		//	ComEventProcess();
		//}
		if(cFlagUsbEvent() )
		{
			//cFlagUsbEvent = 0;
			UsbEventProcess();
		}
		/*
		if(cFlagHostCommandDataReceived == TRUE)
		{
			cFlagHostCommandDataReceived = 0;
			HostCommandDataProcess();
		}
		if(cFlagCom2DataReceived == TRUE)
		{
			cFlagCom2DataReceived = FALSE;
			UartCom2RxProcess();
			//sprintf(cTempString, "hello");
		}*/
		//MCU_Main_Loop();

		if(cUsbConfigured == TRUE)
		{
			Microkernel_Spin();
			JobPipe_ConvertJobResult2UsbStringBuffer();
			if(cUsbReturnJobIsUnfinished == TRUE)
			{
				UsbSendTheLeftJobResult();
			}
			if(SoftTimer0.cTimerAlarm == TRUE)
			{
				if(UsbOutDataBelongs != FREE_DATA)
				{
					switch(UsbOutDataBelongs)
					{
						case PIPE_BUF_PUSH:
							Protocol_PIPE_BUF_PUSH_PART2();		//databelongs flag and time out setting should be clear inside the function
							break;
						case PIPE_BUF_PUSH_PACK:
							Protocol_PIPE_BUF_PUSH_PACK_PART2();
							break;
						default:
							;
							break;
					}
				}
				else
				{
					ClearSoftTimer0();
				}
			}
			if(USB_inbound_USB_data() == TRUE)		//received usb data have been copied to usb rx loop buffer from PMA
			{
				if(UsbOutDataBelongs != FREE_DATA)
				{
					switch(UsbOutDataBelongs)
					{
						case PIPE_BUF_PUSH:
							Protocol_PIPE_BUF_PUSH_PART2();		//databelongs flag and time out setting should be clear inside the function
							break;
						case PIPE_BUF_PUSH_PACK:
							Protocol_PIPE_BUF_PUSH_PACK_PART2();
							break;
						default:
							;
							break;
					}

				}
				else				//Free data means BF command or other, need clarify
				{
					//////////////////////////////////////////
					// If we are master, we'll listen to the USB
					//////////////////////////////////////////
					//if (XLINK_ARE_WE_MASTER)
					//{
					/*		remarked because of usb have data received
					// We listen to USB
					i = 3;
					while (!USB_inbound_USB_data() && i-- > 1);
					
					// Check, if 'i' equals zero, we discard the actual command buffer
					if (i <= 1)
					{
						// Clear buffer, something is wrong...
						i_count = 0;
						sz_cmd[0] = 0;
						sz_cmd[1] = 0;
						sz_cmd[2] = 0;
					}*/
					

					// We've reduced timeout counter to 5000, so we can run this function periodically
					// Flush the job (should they exist)
					// This must be called on a timer running 
					// Management_flush_p2p_buffer_into_engines();

					// Check if there is data, or we just had
					// an overflow?
					//if (!USB_inbound_USB_data()) continue;
					
					// Was EndOfStream detected?
					//bEOSDetected = FALSE;
					intercepted_command_length = 0;
					//iExpectedPacketLength = 0;
					//bInterceptingChainForwardReq = FALSE;
					//bSingleStageJobIssueCommand = FALSE;

					// Read all the data that has arrived
					
					/*   Changed the copy flow
					*/ 
					/*
					while (USB_inbound_USB_data() && i_count < 1024)
					{
						// Read byte
						sz_cmd[i_count] = USB_read_byte();
						
						// Are we a single-cycle job issue?
						bSingleStageJobIssueCommand = ((sz_cmd[0] == 'S') && (sz_cmd[1] == 48)) ? TRUE : FALSE;
						
						// Are we expecting 
						bInterceptingChainForwardReq = (sz_cmd[0] == 64) ? TRUE : FALSE;
						
						if (bInterceptingChainForwardReq) 
						{
							 intercepted_command_length = (i_count + 1) - 3; // First three characters are @XY (X = Packet Size, Y = Forware Number)
							 if (i_count > 1) iExpectedPacketLength = sz_cmd[1] & 0x0FF;
						}	
						
						// Increase Count				 
						i_count++;
						
						// Check if 3-byte packet is done
						if ((i_count == 3) && (bInterceptingChainForwardReq == FALSE) && (bSingleStageJobIssueCommand == FALSE))
						{
							bEOSDetected = TRUE;
							break;
						}					 
						
						if (bInterceptingChainForwardReq && (intercepted_command_length == iExpectedPacketLength))
						{
							bEOSDetected = TRUE;
							break;
						}
						
						if ((bSingleStageJobIssueCommand == TRUE) && (i_count == 48))
						{
							bEOSDetected = TRUE;
							break;
						}
						
						// Check if we've overlapped
						if (i_count > 256)							
						{
							// Clear buffer, something is wrong...
							i_count = 0;
							sz_cmd[0] = 0;
							sz_cmd[1] = 0;
							sz_cmd[2] = 0;
							continue;
						}
						
					}	
					
					// Check for length and signature
					// If we've received less than three characters, continue waiting
					if (i_count < 3)
					{
						if (bEOSDetected == TRUE)
						{
							// Clear buffer, something is wrong...
							i_count = 0;
							sz_cmd[0] = 0;
							sz_cmd[1] = 0;
							sz_cmd[2] = 0;
							continue;
						}
						else 
						{
							continue; // We'll continue...			
						}					
					}*/

					// we copy all usb buffer to sz_cmd first, then find key word, in order to speed up code

					i_count = __ARM_UsbGetData((u8*)sz_cmd, 1024); 
					if((i_count <= 256) && (i_count >=3))
					{
						
						// Are we a single-stage job-issue command?
						//bSingleStageJobIssueCommand = ((sz_cmd[0] == 'S') && (sz_cmd[1] == 48)) ? TRUE : FALSE; 

						//we only operate legal command, unknown command reject
						if((sz_cmd[0] == 'S') && (sz_cmd[1] == 48))		//Single-stage job-issue command
						{
							Protocol_handle_job_single_stage(sz_cmd);
						}
						else if((sz_cmd[0] == '@') && (XLINK_ARE_WE_MASTER))		//chain-forward command request
						{
							// Forward command to the device in chain...
							DEBUG_LastXLINKTransTook = MACRO_GetTickCountRet;
							
							Protocol_chain_forward((char)sz_cmd[2], 
												   (char*)(sz_cmd+3), 
												   intercepted_command_length); // Length is always 3	
												   
							DEBUG_LastXLINKTransTook = MACRO_GetTickCountRet - DEBUG_LastXLINKTransTook;
						}
						else if((sz_cmd[0] == 'Z') && (sz_cmd[2] == 'X'))	//Local command
						{
							switch(sz_cmd[1])
							{
								// Job-Issuing commands
								case PROTOCOL_REQ_BUF_PUSH_JOB:
									Protocol_PIPE_BUF_PUSH();
									break;
								case PROTOCOL_REQ_BUF_PUSH_JOB_PACK:
									Protocol_PIPE_BUF_PUSH_PACK();
									break;
								case PROTOCOL_REQ_HANDLE_JOB:
									Protocol_handle_job();
									break;
							
								// Load String / Save String
								case PROTOCOL_REQ_SAVE_STRING:
									Protocol_save_string();
									break;
								case PROTOCOL_REQ_LOAD_STRING:
									Protocol_load_string();
									break;
							
								// The rest of the commands
								case PROTOCOL_REQ_BUF_FLUSH:
									Protocol_PIPE_BUF_FLUSH();
									break;
								case PROTOCOL_REQ_BUF_FLUSH_EX:
									Protocol_PIPE_BUF_FLUSH_EX();
									break;
								case PROTOCOL_REQ_BUF_STATUS:
									Protocol_PIPE_BUF_STATUS();
									break;
								case PROTOCOL_REQ_INFO_REQUEST:
									Protocol_info_request();
									break;
								case PROTOCOL_REQ_ID:
									Protocol_id();
									break;
								case PROTOCOL_REQ_BLINK:
									Protocol_Blink();
									break;
								case PROTOCOL_REQ_TEMPERATURE:
									Protocol_temperature();
									break;
								case PROTOCOL_REQ_GET_STATUS:
									Protocol_get_status();
									break;
								case PROTOCOL_REQ_GET_VOLTAGES:
									Protocol_get_voltages();
									break;
								case PROTOCOL_REQ_GET_FIRMWARE_VERSION:
									Protocol_get_firmware_version();
									break;
								case PROTOCOL_REQ_SET_FREQ_FACTOR:
									Protocol_set_freq_factor();
									break;
								case PROTOCOL_REQ_GET_FREQ_FACTOR:
									Protocol_get_freq_factor();
									break;
								case PROTOCOL_REQ_SET_XLINK_ADDRESS:
									Protocol_set_xlink_address();
									break;
								case PROTOCOL_REQ_XLINK_ALLOW_PASS:
									Protocol_xlink_allow_pass();
									break;
								case PROTOCOL_REQ_XLINK_DENY_PASS:
									Protocol_xlink_deny_pass();
									break;
								case PROTOCOL_REQ_ECHO:
									Protocol_Echo();
									break;
								case PROTOCOL_REQ_TEST_COMMAND:
									Protocol_Test_Command();
									break;
								case PROTOCOL_REQ_PRESENCE_DETECTION:
									Protocol_xlink_presence_detection();
									break;
								default:
									USB_send_string("ERR:UNKNOWN COMMAND\n");
									break;
							}
						}
						else		//unkown command or data, just ignore
						{
							;
						}
					}
					else		//usb received wrong
					{
						sz_cmd[0] = 0;
						sz_cmd[1] = 0;
						sz_cmd[2] = 0;
						ClearUsbRxBuffer();
					}
					//}
					
					/*
					// Are we a single-stage job-issue command?
					//bSingleStageJobIssueCommand = ((sz_cmd[0] == 'S') && (sz_cmd[1] == 48)) ? TRUE : FALSE; 

					// Check number of bytes received so far.
					// If they are 3, we may have a command here (4 for the AMUX Read)...
					// Reset the count anyway
					//i_count = 0;

					// Check for packet integrity
					
					if (((sz_cmd[0] != 'Z' || sz_cmd[2] != 'X') && (sz_cmd[0] != '@')) && (bSingleStageJobIssueCommand == FALSE)) // @XX means forward to XX (X must be between '0' and '9')
					{
						continue;
					}
					else
					{
						// We have a command. Check for validity
						if ((sz_cmd[0] != '@') &&
							(bSingleStageJobIssueCommand == FALSE) && 
							(sz_cmd[1] != PROTOCOL_REQ_INFO_REQUEST) &&
							(sz_cmd[1] != PROTOCOL_REQ_BUF_FLUSH_EX) &&
							(sz_cmd[1] != PROTOCOL_REQ_HANDLE_JOB) &&
							(sz_cmd[1] != PROTOCOL_REQ_ID) &&
							(sz_cmd[1] != PROTOCOL_REQ_GET_FIRMWARE_VERSION) &&
							(sz_cmd[1] != PROTOCOL_REQ_BLINK) &&
							(sz_cmd[1] != PROTOCOL_REQ_TEMPERATURE) &&
							(sz_cmd[1] != PROTOCOL_REQ_BUF_PUSH_JOB_PACK) &&
							(sz_cmd[1] != PROTOCOL_REQ_BUF_PUSH_JOB) &&
							(sz_cmd[1] != PROTOCOL_REQ_BUF_STATUS) &&
							(sz_cmd[1] != PROTOCOL_REQ_BUF_FLUSH) &&
							(sz_cmd[1] != PROTOCOL_REQ_GET_VOLTAGES) &&
							(sz_cmd[1] != PROTOCOL_REQ_GET_CHAIN_LENGTH) &&
							(sz_cmd[1] != PROTOCOL_REQ_SET_FREQ_FACTOR) &&
							(sz_cmd[1] != PROTOCOL_REQ_GET_FREQ_FACTOR) &&
							(sz_cmd[1] != PROTOCOL_REQ_SET_XLINK_ADDRESS)	&&
							(sz_cmd[1] != PROTOCOL_REQ_XLINK_ALLOW_PASS) &&
							(sz_cmd[1] != PROTOCOL_REQ_XLINK_DENY_PASS) &&
							(sz_cmd[1] != PROTOCOL_REQ_PRESENCE_DETECTION) &&
							(sz_cmd[1] != PROTOCOL_REQ_ECHO) &&
							(sz_cmd[1] != PROTOCOL_REQ_TEST_COMMAND) &&
							(sz_cmd[1] != PROTOCOL_REQ_SAVE_STRING) &&
							(sz_cmd[1] != PROTOCOL_REQ_LOAD_STRING) &&
							(sz_cmd[1] != PROTOCOL_REQ_GET_STATUS))
						{					
							if (XLINK_ARE_WE_MASTER)
							{
								USB_send_string("ERR:UNKNOWN COMMAND\n");
							}						
																			 
							// Continue the loop	
							continue;
						}
						
						// Do we have a Chain-Forward request?
						if ((sz_cmd[0] == '@') && (XLINK_ARE_WE_MASTER))
						{
							// Forward command to the device in chain...
							DEBUG_LastXLINKTransTook = MACRO_GetTickCountRet;
							
							Protocol_chain_forward((char)sz_cmd[2], 
												   (char*)(sz_cmd+3), 
												   intercepted_command_length); // Length is always 3	
												   
							DEBUG_LastXLINKTransTook = MACRO_GetTickCountRet - DEBUG_LastXLINKTransTook;
						}					
						else
						{
							// We have a valid command, go call its procedure...
							if (bSingleStageJobIssueCommand)
							{
								Protocol_handle_job_single_stage(sz_cmd);
							}
							else
							{
								// Job-Issuing commands
								if (sz_cmd[1] == PROTOCOL_REQ_BUF_PUSH_JOB)			Protocol_PIPE_BUF_PUSH();
								if (sz_cmd[1] == PROTOCOL_REQ_BUF_PUSH_JOB_PACK)    Protocol_PIPE_BUF_PUSH_PACK();
								if (sz_cmd[1] == PROTOCOL_REQ_HANDLE_JOB)			Protocol_handle_job();
							
								// Load String / Save String
								if (sz_cmd[1] == PROTOCOL_REQ_SAVE_STRING)			Protocol_save_string();
								if (sz_cmd[1] == PROTOCOL_REQ_LOAD_STRING)			Protocol_load_string();
							
								// The rest of the commands
								if (sz_cmd[1] == PROTOCOL_REQ_BUF_FLUSH)			Protocol_PIPE_BUF_FLUSH();
								if (sz_cmd[1] == PROTOCOL_REQ_BUF_FLUSH_EX)			Protocol_PIPE_BUF_FLUSH_EX();
								if (sz_cmd[1] == PROTOCOL_REQ_BUF_STATUS)			Protocol_PIPE_BUF_STATUS();
								if (sz_cmd[1] == PROTOCOL_REQ_INFO_REQUEST)			Protocol_info_request();
								if (sz_cmd[1] == PROTOCOL_REQ_ID)					Protocol_id();
								if (sz_cmd[1] == PROTOCOL_REQ_BLINK)				Protocol_Blink();
								if (sz_cmd[1] == PROTOCOL_REQ_TEMPERATURE)			Protocol_temperature();
								if (sz_cmd[1] == PROTOCOL_REQ_GET_STATUS)			Protocol_get_status();
								if (sz_cmd[1] == PROTOCOL_REQ_GET_VOLTAGES)			Protocol_get_voltages();
								if (sz_cmd[1] == PROTOCOL_REQ_GET_FIRMWARE_VERSION)	Protocol_get_firmware_version();
								if (sz_cmd[1] == PROTOCOL_REQ_SET_FREQ_FACTOR)		Protocol_set_freq_factor();
								if (sz_cmd[1] == PROTOCOL_REQ_GET_FREQ_FACTOR)		Protocol_get_freq_factor();
								if (sz_cmd[1] == PROTOCOL_REQ_SET_XLINK_ADDRESS)	Protocol_set_xlink_address();
								if (sz_cmd[1] == PROTOCOL_REQ_XLINK_ALLOW_PASS)		Protocol_xlink_allow_pass();
								if (sz_cmd[1] == PROTOCOL_REQ_XLINK_DENY_PASS)		Protocol_xlink_deny_pass();
								if (sz_cmd[1] == PROTOCOL_REQ_ECHO)					Protocol_Echo();
								if (sz_cmd[1] == PROTOCOL_REQ_TEST_COMMAND)			Protocol_Test_Command();
								if (sz_cmd[1] == PROTOCOL_REQ_PRESENCE_DETECTION)   Protocol_xlink_presence_detection();
							}
							
							
						}	
						
						// Once we reach here, our procedure has run and we're back to standby...
					}*/
				}
			}
		}
	}
}

#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
