/*
 * HighLevel_Operations.c
 *
 * Created: 10/01/2013 00:13:49
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
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
#include "HostInteractionProtocols.h"
#include "HighLevel_Operations.h"
#include "FAN_Subsystem.h"
#include "AVR32_OptimizedTemplates.h"
#include "PipeProcessingKernel.h"
#include "bf_peripheral_timer.h"


#include <string.h>
#include <stdio.h>
//#include <avr32/io.h>

int GLOBAL_BLINK_REQUEST;
int XLINK_ARE_WE_MASTER = TRUE;
unsigned int GLOBAL_LastJobResultProduced;
unsigned char GLOBAL_INTERNAL_ASIC_RESET_EXECUTED;



extern unsigned short GLOBAL_ChipActivityLEDCounter[TOTAL_CHIPS_INSTALLED];
extern int  GLOBAL_PULSE_BLINK_REQUEST;
extern unsigned int  GLOBAL_ENGINE_PROCESSING_START_TIMESTAMP[TOTAL_CHIPS_INSTALLED][16]; // When did this engine start processing?
extern unsigned int  GLOBAL_ENGINE_MAXIMUM_OPERATING_TIME[TOTAL_CHIPS_INSTALLED][16];
extern u16 __chip_existence_map[TOTAL_CHIPS_INSTALLED]; // Bit 0 to Bit 16 in each word says if the engine is OK or not...
extern unsigned int __internal_global_iChipCount;
extern unsigned int GLOBAL_TotalEnginesDetectedOnStartup;
extern   char FAN_ActualState;
extern  u32 FAN_ActualState_EnteredTick;

void Microkernel_Spin()
{
	//static   u32 iInitialTimeHolder;
	//u32 iActualTickHolder;
	//static char actualBlinkValue;
	//unsigned char index_hover;
	//char bWasAnyEngineModified;
	//char xChip;
	//unsigned int iDiffVal;
	//char yEngine;
	//char bIsEngineProcessing;
	//char szTempEx[128];
	//unsigned int iNewChipCount;
	//char umz;
	u8 itAttempt;
	// Nothing for the moment
	// Reset Watchdog to prevent system reset. (Timeout for watchdog is 17ms)
	//WATCHDOG_RESET;	
	
	// Job-Pipe Scheduling
	PipeKernel_Spin();	
	
	// Scan XLINK Chain, to be executed every 1.2 seconds

	/*
	//we don't have xlink
	if (XLINK_ARE_WE_MASTER == TRUE)
	{		
		// We refresh the chain every 1.2 seconds
		iInitialTimeHolder = 0;
		  iActualTickHolder = MACRO_GetTickCountRet;
		
		if (iActualTickHolder - iInitialTimeHolder > __XLINK_CHAIN_REFRESH_INTERVAL) 
		{
			iInitialTimeHolder = iActualTickHolder;
			XLINK_MASTER_Refresh_Chain();
		}		
	}	
	*/

	/*
	// Fan-Spin must be executed every 0.5 seconds
	{
		iInitialTimeHolder = 0;
		  iActualTickHolder = MACRO_GetTickCountRet;
		
		if (iActualTickHolder - iInitialTimeHolder > 5000000) // 5,000,000 us = 5sec
		{
			iInitialTimeHolder = iActualTickHolder;
			
			// Call our Fan-Spin
			FAN_SUBSYS_IntelligentFanSystem_Spin();
		}
	}	*/
	/*
	// Global Blink-Request subsystem
	{
		// Blink time holder
		iInitialTimeHolder = 0;
		
		if (GLOBAL_BLINK_REQUEST > 0)
		{
			actualBlinkValue = FALSE;
			  iActualTickHolder = MACRO_GetTickCountRet;
			
			if (iActualTickHolder - iInitialTimeHolder > 30000) // 30,000 us =  30 msec
			{
				iInitialTimeHolder = iActualTickHolder;
				
				// Reverse blink value, but set it to 1 if GLOBAL_BLINK_REQUEST is 1
				if (GLOBAL_BLINK_REQUEST == 1) 
					actualBlinkValue = TRUE;
				else
					actualBlinkValue = (actualBlinkValue == FALSE) ? TRUE : FALSE;
					
				// Now set the LED
				if (actualBlinkValue == TRUE)
				{
					MCU_MainLED_Set();
				}					
				else
				{
					MCU_MainLED_Reset();
				}
									
				// BTW, Reduct he GLOBAL_BLINK_REQUEST
				GLOBAL_BLINK_REQUEST--;
			}
		}
		else
		{
			iInitialTimeHolder = MACRO_GetTickCountRet;
		}			
	}
	*/
	/*
	// Blink Chip LEDs if needed
	#if defined(__PRODUCT_MODEL_LITTLE_SINGLE__) || defined(__PRODUCT_MODEL_JALAPENO__)
	{
		  index_hover = 0;
		
		for (index_hover = 0; index_hover < 8; index_hover++)
		{
			if (CHIP_EXISTS(index_hover))
			{
				if (GLOBAL_ChipActivityLEDCounter[index_hover] != 0) 
				{
					MCU_LED_Reset(index_hover+1);  // LEDs are 1based
					GLOBAL_ChipActivityLEDCounter[index_hover]--; 
				} 
				else 
				{ 
					MCU_LED_Set(index_hover+1);   // LEDs are 1based
				}
			}				
		}			
	}
	#endif
	*/
	/*
	// Should we Pulse?
	{
		if (GLOBAL_PULSE_BLINK_REQUEST != 0)
		{
			// Ok, we turn the LED OFF if 200ms if left until the Pulse Request Expires
			// First, is GlobalPulseRequest already expired?
			iDiffVal = MACRO_GetTickCountRet - GLOBAL_PULSE_BLINK_REQUEST;
			
			// if iDiffVal > 1,000,000 it means it's over, and we'll reset it...			
			// if iDiffVal <   900,000 we'll keep the LED ON
			// if iDiffVal >   900,000 we'll keep the LED OFF
			if (iDiffVal > 1000000)
			{
				// Turn the LED on and Reset the GLOBAL_PULSE_BLINK_REQUEST
				MCU_MainLED_Set();				
				GLOBAL_PULSE_BLINK_REQUEST = 0;
			}
			else
			{
				if (iDiffVal < 900000)
				{
					MCU_MainLED_Set(); // We leave it on...
				}	
				else
				{
					MCU_MainLED_Reset(); // We'll turn it off...					
				}
			}		
		}
	}
	*/
	
	// Engine monitoring Authority
	#if defined(__ENGINE_AUTHORITIVE_ACTIVITY_SUPERVISION)
	{
		// We perform this run every 200th attempt
		static int iActualAttempt = 0;
		iActualAttempt++;
		
		if (iActualAttempt == 200)	
		{
			// Reset
			iActualAttempt = 0;
			
			// If anything was modified, we have to recalculate nonce-range
			//bWasAnyEngineModified = FALSE;
			
			// Now we check the array. Is any active engine taking ENGINE_MAXIMUM_BUSY_TIME to complete? If so, cut it off
			for (xChip = 0; xChip < TOTAL_CHIPS_INSTALLED; xChip++)
			{
				// Does the chip exist?
				if (!CHIP_EXISTS(xChip)) continue;
				
				// Now check the engines
				for (yEngine = 0; yEngine < 16; yEngine++)
				{
					#if defined(DO_NOT_USE_ENGINE_ZERO)
						if (yEngine == 0) continue;
					#endif
					
					// Is engine in use?
					if (!IS_PROCESSOR_OK(xChip, yEngine)) continue; 
					
					// Ok, now check the information. If this engine
					// is active mining and it has been running for more than ENGINE_MAXIMUM_BUSY_TIME then cut it off
					bIsEngineProcessing = TRUE;//ASIC_is_engine_processing(xChip, yEngine);
					
					// Check operating time
					if (bIsEngineProcessing == TRUE)
					{
						if ((unsigned int)(MACRO_GetTickCountRet - GLOBAL_ENGINE_PROCESSING_START_TIMESTAMP[xChip][yEngine]) > GLOBAL_ENGINE_MAXIMUM_OPERATING_TIME[xChip][yEngine] + __ENGINE_OPERATING_TIME_OVERHEAD)
						{
							// Deactivate it
							#if defined(DECOMMISSION_ENGINES_IF_LATE)
								DECOMMISSION_PROCESSOR(xChip, yEngine);
								
								#if defined(__SHOW_DECOMMISSIONED_ENGINES_LOG)
								//char szTempEx[128];
								sprintf(szTempEx,"CHIP %d ENGINE %d DECOMMISSIONED!\n", xChip, yEngine);
								strcat(szDecommLog, szTempEx);
								#endif
								
								bWasAnyEngineModified = TRUE;
							#else
								// Just reset it, but don't do anything bad :)
								ASIC_reset_engine(xChip, yEngine);
							#endif
						}							
					}
				}
				
				// Decomission chips if they have zero processors left
				if (ASIC_get_chip_processor_count(xChip) == 0)
				{
					__chip_existence_map[xChip] = 0;
					__internal_global_iChipCount -= 1; // A chip was decommissioned
				}
			}
			
			// Did we deactivate any engine? If so, we need to recalculate nonce-range for engines (Only if we are NOT running one engine per chip
			#if !defined(QUEUE_OPERATE_ONE_JOB_PER_CHIP)
				if (bWasAnyEngineModified == TRUE)
				{
					// First, update the chip count if necessary
					iNewChipCount = 0;
					for (char umz = 0; umz < TOTAL_CHIPS_INSTALLED; umz++)
					{
						if ((__chip_existence_map[umz]) != 0) iNewChipCount++;
					}
					__internal_global_iChipCount = iNewChipCount; // This will affect the ASIC_get_chip_count function
					
									
					// Proceed with nonce calculation
					ASIC_calculate_engines_nonce_range();
				}
			#endif
		}
	}
	#endif	
	
	// Also update the LEDs (on small board model)
	#if defined(__PRODUCT_MODEL_LITTLE_SINGLE__) || defined(__PRODUCT_MODEL_JALAPENO__)
		itAttempt = 0;
		
		if (itAttempt == 150)
		{
			itAttempt = 0;
			if (ASIC_does_chip_exist(0) == TRUE) MCU_LED_Set(1); else MCU_LED_Reset(1);
			if (ASIC_does_chip_exist(1) == TRUE) MCU_LED_Set(2); else MCU_LED_Reset(2);
			if (ASIC_does_chip_exist(2) == TRUE) MCU_LED_Set(3); else MCU_LED_Reset(3);
			if (ASIC_does_chip_exist(3) == TRUE) MCU_LED_Set(4); else MCU_LED_Reset(4);
			if (ASIC_does_chip_exist(4) == TRUE) MCU_LED_Set(5); else MCU_LED_Reset(5);
			if (ASIC_does_chip_exist(5) == TRUE) MCU_LED_Set(6); else MCU_LED_Reset(6);
			if (ASIC_does_chip_exist(6) == TRUE) MCU_LED_Set(7); else MCU_LED_Reset(7);
			if (ASIC_does_chip_exist(7) == TRUE) MCU_LED_Set(8); else MCU_LED_Reset(8);			
		}
		else
		{
			itAttempt++;
		}
	#endif
		
	#if defined(GENERAL_ASIC_RESET_ON_LOW_ENGINE_COUNT)
		// Check the time from last 
		// What was the last job produced on Queue? Was it more than like 1 minute ago?
		if (MACRO_GetTickCountRet - GLOBAL_LastJobResultProduced > 60000000) 
		{
			if (ASIC_get_processor_count() < (GLOBAL_TotalEnginesDetectedOnStartup / 2))
			{
				// Say what we've done
				GLOBAL_INTERNAL_ASIC_RESET_EXECUTED = TRUE;
				
				// Reset the ASICs -- Something must have gone wrong...
				init_ASIC();
			}			
		}		
	#endif	
}

