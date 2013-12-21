/*
 * MCU_Initialization.c
 *
 * Created: 08/10/2012 21:47:25
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */ 
#include "stm32f10x.h"
#include "AVR32X\AVR32_Module.h"
//#include "PIC32\PIC32_Module.h"
//#include "STM32\STM32_Module.h"
#include "std_defs.h"

/// ************************ MCU Initialization
void init_mcu(void)
{
		__AVR32_LowLevelInitialize();

}
