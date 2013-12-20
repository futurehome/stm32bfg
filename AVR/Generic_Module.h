/*
 * Generic_Module.h
 *
 * Created: 12/10/2012 00:10:54
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */ 
#ifndef GENERIC_MODULE_H_
#define GENERIC_MODULE_H_

// Now it depends which MCU we have chosen
// General MCU Functions
void MCU_LowLevelInitialize(void);

// A2D Functions
void  MCU_A2D_Initialize(void);
void  MCU_A2D_SetAccess(void);
int MCU_A2D_GetTemp1 (void);
int MCU_A2D_GetTemp2 (void);
int MCU_A2D_Get3P3V  (void);
int MCU_A2D_Get1V(void);
int MCU_A2D_GetPWR_MAIN(void);

// USB Chip Functions
void	MCU_USB_Initialize(void);
void	MCU_USB_SetAccess(void);

char	MCU_USB_WriteData(char* iData, unsigned int iCount);
char	MCU_USB_GetData(char* iData, unsigned int iMaxCount);
char	MCU_USB_GetInformation(void);

void	MCU_USB_FlushInputData(void);
void	MCU_USB_FlushOutputData(void);
 
// XLINK Functions
void	MCU_CPLD_Initialize(void);
void	MCU_CPLD_SetAccess(void);
void	MCU_CPLD_Write(char iAdrs, char iData);
char    MCU_CPLD_Read(char iAdrs);

// SC Chips
void	MCU_SC_Initialize(void);
void	MCU_SC_SetAccess(void);
unsigned int MCU_SC_GetDone  (char iChip);
unsigned int MCU_SC_ReadData (char iChip, char iEngine, unsigned char iAdrs);
unsigned int MCU_SC_WriteData(char iChip, char iEngine, unsigned char iAdrs, unsigned int iData);

// Main LED
void	MCU_MainLED_Initialize(void);
void	MCU_MainLED_Set(void);
void	MCU_MainLED_Reset(void);

// LEDs
void	MCU_LED_Initialize(void);
void	MCU_LED_SetAccess(void);
void	MCU_LED_Set(char iLed);
void	MCU_LED_Reset(char iLed);

// Timer
void    MCU_Timer_Initialize(void);
void    MCU_Timer_SetInterval(unsigned int iPeriod);
void	MCU_Timer_Start(void);
void	MCU_Timer_Stop(void);
int		MCU_Timer_GetValue(void);

// FAN unit
void	MCU_FAN_Initialize(void);
void	MCU_FAN_SetAccess(void);
void	MCU_FAN_SetSpeed(char iSpeed);

// Some ASIC definitions
void    __MCU_ASIC_Activate_CS(void);
void    __MCU_ASIC_Deactivate_CS(void);


#endif /* GENERIC_MODULE_H_ */
