/*
 * FAN_Subsystem.c
 *
 * Created: 11/02/2013 23:10:36
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */ 

// Include standard definitions
#include "stm32f10x.h"
#include "std_defs.h"
#include "Generic_Module.h"
#include "ChainProtocol_Module.h"
#include <string.h>
#include "AVR32X\AVR32_Module.h"
//#include <avr32/io.h>
#include "AVR32_OptimizedTemplates.h"
#include "FAN_Subsystem.h"
#include "ASIC_Engine.h"
#include "bf_peripheral_timer.h"
#include "bf_general.h"



char GLOBAL_CRITICAL_TEMPERATURE;
  char FAN_ActualState;
  u32 FAN_ActualState_EnteredTick;


// Now to our codes
  void FAN_SUBSYS_Initialize(void)
{
	// Initialize state to 0
	/*
	__AVR32_FAN_Initialize();
	FAN_SUBSYS_SetFanState(FAN_STATE_AUTO);			
	GLOBAL_CRITICAL_TEMPERATURE = FALSE;
	*/
}

  void FAN_SUBSYS_IntelligentFanSystem_Spin(void)
{
	// We execute this function every 50th call
	/*
	static   char __attempt = 0;
	
	if (__attempt++ < 10) return;
	
	// It is the 50th call
	__attempt = 0;
	*/
	
	// Check temperature
	  //int iTemp1 = __AVR32_A2D_GetTemp1();
	  //int iTemp2 = __AVR32_A2D_GetTemp2();
	  int iTemp1 = __ARM_A2D_GetTemp1();
	  int iTemp2 = __ARM_A2D_GetTemp2();
	  int iTempAveraged = (iTemp1 > iTemp2) ? iTemp1 : iTemp2; // (iTemp2 + iTemp1) / 2;
	
	if (iTempAveraged > 90)
	{
		// Holy jesus! We're in a critical situation...
		GLOBAL_CRITICAL_TEMPERATURE = TRUE;
	}
	else
	{
		if (GLOBAL_CRITICAL_TEMPERATURE == TRUE)
		{
			if (iTempAveraged < 60) // Hysterysis
			{ 
				GLOBAL_CRITICAL_TEMPERATURE = FALSE;
				
				// Also, restart the ASICs
				#if defined(__ASICS_RESTART_AFTER_HIGH_TEMP_RECOVERY)
					init_ASIC();				
				#endif
			}
		}
		else
		{
			// If we're here, it means we're not critical anymore
			GLOBAL_CRITICAL_TEMPERATURE = FALSE;			
		}
	}	
	
	// Do we remain at full speed? If so, get it done and return
	#if defined(FAN_SUBSYSTEM_REMAIN_AT_FULL_SPEED)
		__ARM_FAN_SetSpeed(FAN_CONTROL_BYTE_REMAIN_FULL_SPEED);
		return;
	#endif
	
	// Are we close to the critical temperature? Override FAN if necessary
	if (iTempAveraged > 70)
	{
		// Override fan, set it to maximum
		__ARM_FAN_SetSpeed(FAN_CONTROL_BYTE_VERY_FAST);
		
		// We're done. The device will no longer process nonces
		return;
	}
	
	// Ok, now set the FAN speed according to our setting
	if (FAN_ActualState == FAN_STATE_VERY_SLOW)
	{
		// Set the fan speed
		__ARM_FAN_SetSpeed(FAN_CONTROL_BYTE_VERY_SLOW);
		return;
	}
	else if (FAN_ActualState == FAN_STATE_SLOW)
	{
		// Set the fan speed
		__ARM_FAN_SetSpeed(FAN_CONTROL_BYTE_SLOW);
		return;
	}
	else if (FAN_ActualState == FAN_STATE_MEDIUM)
	{
		// Set the fan speed
		__ARM_FAN_SetSpeed(FAN_CONTROL_BYTE_MEDIUM);
		return;		
	}
	else if (FAN_ActualState == FAN_STATE_FAST)	
	{
		// Set the fan speed
		__ARM_FAN_SetSpeed(FAN_CONTROL_BYTE_FAST);
		return;		
	}
	else if (FAN_ActualState == FAN_STATE_VERY_FAST)
	{
		// Set the fan speed
		__ARM_FAN_SetSpeed(FAN_CONTROL_BYTE_VERY_FAST);
		return;		
	}
	
	// We're in AUTO mode... There are rules to respect form here...
	if (iTempAveraged <= 30)
	{
		__ARM_FAN_SetSpeed(FAN_CONTROL_BYTE_VERY_SLOW);
	}		
	else if ((iTempAveraged > 35) && (iTempAveraged <= 42))
	{
		__ARM_FAN_SetSpeed(FAN_CONTROL_BYTE_SLOW);
	}		
	else if ((iTempAveraged > 45) && (iTempAveraged <= 53))
	{
		__ARM_FAN_SetSpeed(FAN_CONTROL_BYTE_MEDIUM);	
	}		
	else if ((iTempAveraged > 57) && (iTempAveraged <= 67))
	{
		__ARM_FAN_SetSpeed(FAN_CONTROL_BYTE_FAST);		
	}		
	else if (iTempAveraged > 70)
	{	
		__ARM_FAN_SetSpeed(FAN_CONTROL_BYTE_VERY_FAST);		
	}		
		
	// Ok, We're done...
}

  void FAN_SUBSYS_SetFanState(char iState)
{
	FAN_ActualState = iState;
	FAN_ActualState_EnteredTick = MACRO_GetTickCountRet;
}

