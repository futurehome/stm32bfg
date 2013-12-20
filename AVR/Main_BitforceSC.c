/*
 *  Created on: Nov 8, 2011
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */

/* BUG FIX LOG
 * ------------------------------------------------------------------------------------------------
 * July 3rd 2012 - Fixed the P2P JOB problem (range_end - range_begin, which was inverse)
 * June 2nd 2012 - Realized the system must run a 16MHz to ensure compatibility with flash SPI
 *
 */
 #include "stm32f10x.h"
#include "std_defs.h"
#include "MCU_Initialization.h"
#include "JobPipe_Module.h"
#include "Generic_Module.h"
#include "ChainProtocol_Module.h"
#include "USBProtocol_Module.h"
#include "A2D_Module.h"
#include "ASIC_Engine.h"
//#include <avr32/io.h>
#include "JohnChengASIC.h"

#include <string.h>
#include <stdio.h>

#include "HostInteractionProtocols.h"
#include "HighLevel_Operations.h"
#include "AVR32_OptimizedTemplates.h"
#include "AVR32X/AVR32_Module.h"
#include "FAN_Subsystem.h"
#include "Main_BitforceSC.h"
#include "bf_general.h"
#include "bf_peripheral_timer.h"


// Information about the result we're holding
extern buf_job_result_packet  __buf_job_results[PIPE_MAX_BUFFER_DEPTH];
//extern char 		   __buf_job_results_count;  // Total of results in our __buf_job_results
extern unsigned short GLOBAL_ChipActivityLEDCounter[TOTAL_CHIPS_INSTALLED];
extern unsigned int GLOBAL_TotalEnginesDetectedOnStartup;
extern unsigned int GLOBAL_LastJobIssueToAllEngines;
extern int XLINK_ARE_WE_MASTER;
extern int GLOBAL_BLINK_REQUEST;
extern int  GLOBAL_PULSE_BLINK_REQUEST;
extern unsigned char GLOBAL_INTERNAL_ASIC_RESET_EXECUTED;
extern char sz_cmd[1024];


// -------------------------------------
// ========== META FEATURES ============
// -------------------------------------



// -------------------------------------
// ============ END META ===============
// -------------------------------------


/////////////////////////////////////////////////////////
/// PROTOCOL
/////////////////////////////////////////////////////////
//#define FAN_CONTROL_BYTE_VERY_SLOW			(FAN_CTRL3)
//#define FAN_CONTROL_BYTE_SLOW				(FAN_CTRL2)
//#define FAN_CONTROL_BYTE_MEDIUM				(FAN_CTRL2 | FAN_CTRL3)
//#define FAN_CONTROL_BYTE_FAST				(FAN_CTRL0)
//#define FAN_CONTROL_BYTE_VERY_FAST			(FAN_CTRL0 | FAN_CTRL1)

#define FAN_CONTROL_BYTE_REMAIN_FULL_SPEED	(FAN_CTRL0 | FAN_CTRL1 | FAN_CTRL2 | FAN_CTRL3)	// Turn all mosfets off...

#define FAN_CTRL0	 0x01
#define FAN_CTRL1	 0x02
#define FAN_CTRL2	 0x04
#define FAN_CTRL3	 0x08

int global_vals[6];
unsigned int DEBUG_LastXLINKTransTook;



/*!
 * brief Main function. Execution starts here.
 */
void AVR_main(void)
{
	//unsigned int iHolder;
	//unsigned int iNonceValues[16];
	//unsigned int iNonceCount;
	u32 dwDelay;
	// Initialize everything here...
	//init_mcu();			//useless for ARM

	// Continue...
	//init_mcu_led();		//no led
	JobPipe_init();
	//init_XLINK();		//no xlink
	//init_USB();			//useless
	
	// Initialize A2D
	/*					//useless
	a2d_init();
	a2d_get_temp(0);    // This is to clear the first invalid conversion result...
	a2d_get_temp(1);    // This is to clear the first invalid conversion result...
	a2d_get_voltage(0); // This is to clear the first invalid conversion result...
	a2d_get_voltage(1); // This is to clear the first invalid conversion result...
	a2d_get_voltage(2); // This is to clear the first invalid conversion result...
	*/
	
	// Initialize timer
	//MCU_Timer_Initialize();		//==will be replaced by ARM timer
	//MCU_Timer_SetInterval(10);			//==will be replaced by ARM timer
	//MCU_Timer_Start();				//==will be replaced by ARM timer
	
	// Turn on the front LED
	MCU_MainLED_Initialize();		//==will adding code
	MCU_MainLED_Set();				
	
	// Initialize flash-saving sequence
	//	__AVR32_Flash_Initialize();		//no code
	
	// Initialize FAN subsystem
	FAN_SUBSYS_Initialize();		//==will adding code
	
	// Initialize Global Activity Chip LEDs
	GLOBAL_ChipActivityLEDCounter[0] = 0;
	GLOBAL_ChipActivityLEDCounter[1] = 0;
	GLOBAL_ChipActivityLEDCounter[2] = 0;
	GLOBAL_ChipActivityLEDCounter[3] = 0;
	GLOBAL_ChipActivityLEDCounter[4] = 0;
	GLOBAL_ChipActivityLEDCounter[5] = 0;
	GLOBAL_ChipActivityLEDCounter[6] = 0;
	GLOBAL_ChipActivityLEDCounter[7] = 0;
	
	// Reset the total number of engines detected on startup
	GLOBAL_TotalEnginesDetectedOnStartup = 0;
	
	// Last time JobIssue was called. 
	GLOBAL_LastJobIssueToAllEngines = 0;
	
	// Wait for 500ms before doing anything		//seems SysTick = 1uS
	//  iHolder = MACRO_GetTickCountRet;										//==will fixed later
	//while (MACRO_GetTickCountRet + 2 - iHolder < 500000) WATCHDOG_RESET;
	//__MCU_ASIC_Activate_CS();
	//__MCU_ASIC_Deactivate_CS();
	dwDelay = 0x40ffff;
			while((dwDelay--) != 0)
			{
			}
	// Perform an ASIC GET CHIP COUNT
	//	__MCU_ASIC_Activate_CS();
	//__MCU_ASIC_Deactivate_CS();
	init_ASIC();								//==will fixed later
	
	// Now set the side-led's accordingly
	#if defined(__PRODUCT_MODEL_LITTLE_SINGLE) || defined(__PRODUCT_MODEL_JALAPENO)
		if (ASIC_does_chip_exist(0) == TRUE) MCU_LED_Set(1);	//==will fixed later
		if (ASIC_does_chip_exist(1) == TRUE) MCU_LED_Set(2);		//==will fixed later
		if (ASIC_does_chip_exist(2) == TRUE) MCU_LED_Set(3);
		if (ASIC_does_chip_exist(3) == TRUE) MCU_LED_Set(4);
		if (ASIC_does_chip_exist(4) == TRUE) MCU_LED_Set(5);
		if (ASIC_does_chip_exist(5) == TRUE) MCU_LED_Set(6);
		if (ASIC_does_chip_exist(6) == TRUE) MCU_LED_Set(7);
		if (ASIC_does_chip_exist(7) == TRUE) MCU_LED_Set(8);	//==will fixed later
	#endif
	

	
	// Detect if we're chain master or not [MODIFY]
	//XLINK_ARE_WE_MASTER = TRUE; //XLINK_detect_if_we_are_master(); // For the moment we're the chain master [MODIFY]
	if (XLINK_ARE_WE_MASTER) 
	{ 
		// Wait for a small time
		blink_medium(); 			//==will fixed later
		WATCHDOG_RESET;
		blink_medium(); 
		WATCHDOG_RESET;
		blink_medium();
		WATCHDOG_RESET; 
		blink_medium();
		WATCHDOG_RESET; 
		blink_medium();
		WATCHDOG_RESET; 
		blink_medium();
		WATCHDOG_RESET; 
		
		// Initialize the XLINK. Interrogate all devices in the chain and assign then addresses
		/*			//==no xlink
		if (XLINK_is_cpld_present() == TRUE)
		{
			// We're the master, set proper configuration
			XLINK_set_cpld_id(0);
			XLINK_set_cpld_master(TRUE);
			XLINK_set_cpld_passthrough(FALSE);
			
			if (XLINK_MASTER_Start_Chain() == FALSE)
			{
				// Ok this can be bad, we failed the chain initialization
			}
		}*/
	}
	/*
	else
	{
		if (XLINK_is_cpld_present() == TRUE)
		{
			// Disable pass-through and set our cpld-address = 0x1E
			// We will await enumeration
			XLINK_set_cpld_id(XLINK_GENERAL_DISPATCH_ADDRESS);
			XLINK_set_cpld_master(FALSE);
			XLINK_set_cpld_passthrough(FALSE);
		}
	}*/
	
	// Reset global values
	global_vals[0] = 0;
	global_vals[1] = 0;
	global_vals[2] = 0;
	global_vals[3] = 0;
	global_vals[4] = 0;
	global_vals[5] = 0;

	GLOBAL_BLINK_REQUEST = 0;
	GLOBAL_PULSE_BLINK_REQUEST = 0;
	
	// Clear ASIC Results... This will make sure the diagnostic nonces are cleared

	//ASIC_get_job_status(iNonceValues, &iNonceCount, FALSE, 0);					//==will fixed later
	
	// Did we reset the ASICs internally?
	GLOBAL_INTERNAL_ASIC_RESET_EXECUTED = FALSE;
	
	// Go to our protocol main loop
	//MCU_Main_Loop();
	//return(0);
}


//////////////////////////////////
//// PROTOCOL functions
//////////////////////////////////

// OK, a bit of explanation here:
// The only operation that can be in progress is Job-Handling.
// We can do one thing however, make sure that the job is not active...
// We must read the ASIC periodically until we have some response.
void MCU_Main_Loop()
{
	//char sz_cmd[1024];
	//unsigned int umx;
	unsigned int  i_count;
	
	//char bTimeoutDetectedOnXLINK;
	char bDeviceNotRespondedOnXLINK;
	unsigned int bEOSDetected;
	unsigned int iExpectedPacketLength;
	unsigned int bInterceptingChainForwardReq;
	unsigned char bSingleStageJobIssueCommand;
	//char bSingleStageJobIssueCommand;
	//unsigned int iExpectedPacketLength;
	//unsigned int bInterceptingChainForwardReq;
	//unsigned char bSingleStageJobIssueCommand;
	// Commands received from PC is 3 bytes in length
	// and will look like 'Z' + [COMMAND] + 'X'.
	//
	// Once we receive a packet like this, we must respond with 'OK'
	// and call the corresponding function.
	// The function will take it from there...
	// (PC Will send the rest of the data after that...)

	// First things first, we must clear the FTDI chip
	// Read all that you can..
	  int i = 10000;
	unsigned int intercepted_command_length = 0;
	
	//while (USB_inbound_USB_data() && i-- > 1) USB_read_byte();

	//Clear FTDI chip instead by clear arm usb receive buffer.
	//ClearUsbRxBuffer();

	// OK, now the memory on FTDI is empty,
	// wait for standard packet size
	//char sz_cmd[1024];
	//unsigned int umx;
	i_count = 0;
	
	//bTimeoutDetectedOnXLINK = 0;
	bDeviceNotRespondedOnXLINK = 0;			
	
	//for (umx = 0; umx < 1024; umx++) sz_cmd[umx] = 0;
	
	// Clear logg buffer if needed
	#if defined(__SHOW_DECOMMISSIONED_ENGINES_LOG)
		strcpy(szDecommLog,"");	
	#endif
	
	//////////////////////////////////////////
	// Turn the LED on
	//////////////////////////////////////////
	//MCU_MainLED_Set();
	
	while (1)
	{
		// HighLevel Functions Spin
		Microkernel_Spin();

		//////////////////////////////////////////
		// If we are master, we'll listen to the USB
		//////////////////////////////////////////
		if (XLINK_ARE_WE_MASTER)
		{
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
			}

			// We've reduced timeout counter to 5000, so we can run this function periodically
			// Flush the job (should they exist)
			// This must be called on a timer running 
			// Management_flush_p2p_buffer_into_engines();

			// Check if there is data, or we just had
			// an overflow?
			if (!USB_inbound_USB_data()) continue;
			
			// Was EndOfStream detected?
			bEOSDetected = FALSE;
			intercepted_command_length = 0;
			iExpectedPacketLength = 0;
			bInterceptingChainForwardReq = FALSE;
			bSingleStageJobIssueCommand = FALSE;

			// Read all the data that has arrived
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
			}
		}
		/*
		else 
		{
			///////////////////////////////////////////////
			// We listen to XLINK, as we are a chain-slave
			///////////////////////////////////////////////
						
			// Wait for incoming transactions
			bTimeoutDetectedOnXLINK = FALSE;
			
			// Run the procedure
			XLINK_SLAVE_wait_transact(sz_cmd, 
									  &i_count, 
									  256, 
									  1000,  // 1000us, or 1ms
									  &bTimeoutDetectedOnXLINK, 
									  FALSE, TRUE); // Note: WE ARE WAITING FOR COMMAND
			
			// Check for sz_cmd, if it's PUSH then we have an invalid command
			if ((sz_cmd[0] == 'P') && (sz_cmd[1] == 'U') && (sz_cmd[2] == 'S') && (sz_cmd[3] == 'H'))
			{
				MACRO_XLINK_send_packet(XLINK_get_cpld_id(), "INVA", 4, TRUE, FALSE);
				continue;
			}
			
			// Check for sz_cmd, AA BB C4 <ID>, then we set CPLD ID and enable pass-through
			if ((sz_cmd[0] == 0xAA) && (sz_cmd[1] == 0xBB) && (sz_cmd[2] == 0xC4) && (i_count == 4))
			{
				// We respond with 'ACK' and then change the address
				XLINK_SLAVE_respond_transact("ACK", 
											 3,
											 __XLINK_TRANSACTION_TIMEOUT__, 
											 &bTimeoutDetectedOnXLINK, 
											 FALSE); // Note: We'll used XLINK general dispatch address for this specific operation!

				// Set the new address
				XLINK_set_cpld_id(sz_cmd[3]);
				continue;
			}
			
			// Check 
			if (bTimeoutDetectedOnXLINK) continue;			
		}
		*/
		
		// Are we a single-stage job-issue command?
		bSingleStageJobIssueCommand = ((sz_cmd[0] == 'S') && (sz_cmd[1] == 48)) ? TRUE : FALSE; 

		// Check number of bytes received so far.
		// If they are 3, we may have a command here (4 for the AMUX Read)...
		if (TRUE)
		{
			// Reset the count anyway
			i_count = 0;

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
					else
					{
						XLINK_SLAVE_respond_transact("ERR:UNKNOWN COMMAND\n", 
													 sizeof("ERR:UNKNOWN COMMAND\n"), 
													 __XLINK_TRANSACTION_TIMEOUT__, 
													 &bDeviceNotRespondedOnXLINK, 
													 FALSE);
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
			}
		}
		else
		{
			i_count = 0;
			continue;
		}		
	}
}

