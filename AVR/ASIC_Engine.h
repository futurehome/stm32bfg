/*
 * ASIC_Engine.h
 *
 * Created: 20/11/2012 23:17:01
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */ 

#ifndef ASIC_ENGINE_H_
#define ASIC_ENGINE_H_

#include "bf_general.h"

#define ASIC_JOB_IDLE				0
#define ASIC_JOB_NONCE_PROCESSING	1
#define ASIC_JOB_NONCE_FOUND		2
#define ASIC_JOB_NONCE_NO_NONCE		3

// Counter low-dword and high-dword
#define ASIC_SPI_MAP_COUNTER_LOW_LWORD	0x40
#define ASIC_SPI_MAP_COUNTER_LOW_HWORD  0x41
#define ASIC_SPI_MAP_COUNTER_HIGH_LWORD	0x42
#define ASIC_SPI_MAP_COUNTER_HIGH_HWORD 0x43

#define ASIC_SPI_MAP_H0_A_LWORD   0x80
#define ASIC_SPI_MAP_H0_A_HWORD   0x81
#define ASIC_SPI_MAP_H0_B_LWORD   0x82  
#define ASIC_SPI_MAP_H0_B_HWORD   0x83  
#define ASIC_SPI_MAP_H0_C_LWORD   0x84  
#define ASIC_SPI_MAP_H0_C_HWORD   0x85  
#define ASIC_SPI_MAP_H0_D_LWORD   0x86  
#define ASIC_SPI_MAP_H0_D_HWORD   0x87  
#define ASIC_SPI_MAP_H0_E_LWORD   0x88  
#define ASIC_SPI_MAP_H0_E_HWORD   0x89  
#define ASIC_SPI_MAP_H0_F_LWORD   0x8A  
#define ASIC_SPI_MAP_H0_F_HWORD   0x8B  
#define ASIC_SPI_MAP_H0_G_LWORD   0x8C  
#define ASIC_SPI_MAP_H0_G_HWORD   0x8D  
#define ASIC_SPI_MAP_H0_H_LWORD   0x8E  
#define ASIC_SPI_MAP_H0_H_HWORD   0x8F  

#define ASIC_SPI_MAP_H1_A_LWORD   0x90  
#define ASIC_SPI_MAP_H1_A_HWORD   0x91  
#define ASIC_SPI_MAP_H1_B_LWORD   0x92  
#define ASIC_SPI_MAP_H1_B_HWORD   0x93  
#define ASIC_SPI_MAP_H1_C_LWORD   0x94  
#define ASIC_SPI_MAP_H1_C_HWORD   0x95  
#define ASIC_SPI_MAP_H1_D_LWORD   0x96  
#define ASIC_SPI_MAP_H1_D_HWORD   0x97  
#define ASIC_SPI_MAP_H1_E_LWORD   0x98  
#define ASIC_SPI_MAP_H1_E_HWORD   0x99  
#define ASIC_SPI_MAP_H1_F_LWORD   0x9A  
#define ASIC_SPI_MAP_H1_F_HWORD   0x9B  
#define ASIC_SPI_MAP_H1_G_LWORD   0x9C  
#define ASIC_SPI_MAP_H1_G_HWORD   0x9D  
#define ASIC_SPI_MAP_H1_H_LWORD   0x9E  
#define ASIC_SPI_MAP_H1_H_HWORD   0x9F  

#define ASIC_SPI_MAP_BARRIER_LWORD   0x6E   // Must be set to 0x0FFFF FF7F (-129)
#define ASIC_SPI_MAP_BARRIER_HWORD   0x6F  

#define ASIC_SPI_MAP_W0_LWORD		  0xA0    // Continues as W0_HWORD, W1_LWORD, W1_HWORD, ... Up to W31

#define ASIC_SPI_MAP_LIMITS_LWORD	  0xA6   // Set to 0 by default
#define ASIC_SPI_MAP_LIMITS_HWORD	  0xA7   // Set to 0 by default

#define ASIC_SPI_READ_STATUS_REGISTER	0x00  
#define ASIC_SPI_WRITE_REGISTER			0x00  

#define ASIC_SPI_CLOCK_OUT_ENABLE		0x61  
#define ASIC_SPI_OSC_CONTROL			0x60  

#define ASIC_SPI_FIFO0_LWORD	  0x80   // This is the fifo0 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H0...)
#define ASIC_SPI_FIFO0_HWORD	  0x81   // This is the fifo0 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H0...)
#define ASIC_SPI_FIFO1_LWORD	  0x82   // This is the fifo1 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H1...)
#define ASIC_SPI_FIFO1_HWORD	  0x83   // This is the fifo1 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H1...)
#define ASIC_SPI_FIFO2_LWORD	  0x84   // This is the fifo2 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H2...)
#define ASIC_SPI_FIFO2_HWORD	  0x85   // This is the fifo2 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H2...)
#define ASIC_SPI_FIFO3_LWORD	  0x86   // This is the fifo3 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H3...)
#define ASIC_SPI_FIFO3_HWORD	  0x87   // This is the fifo3 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H3...)
#define ASIC_SPI_FIFO4_LWORD	  0x88   // This is the fifo4 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H4...)
#define ASIC_SPI_FIFO4_HWORD	  0x89   // This is the fifo4 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H4...)
#define ASIC_SPI_FIFO5_LWORD	  0x8A   // This is the fifo5 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H5...)
#define ASIC_SPI_FIFO5_HWORD	  0x8B   // This is the fifo5 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H5...)
#define ASIC_SPI_FIFO6_LWORD	  0x8C   // This is the fifo6 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H6...)
#define ASIC_SPI_FIFO6_HWORD	  0x8D   // This is the fifo6 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H6...)
#define ASIC_SPI_FIFO7_LWORD	  0x8E   // This is the fifo7 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H7...)
#define ASIC_SPI_FIFO7_HWORD	  0x8F   // This is the fifo7 address (Lower 16 Bit) (it applies when reading, so no conflict with MAP_H7...)

#define ASIC_SPI_GLOBAL_QUERY_LISTEN	0xFF // This tells us which engine has finished...

// Read Status Register
#define ASIC_SPI_READ_STATUS_FIFO_DEPTH8_BIT		0x8000    
#define ASIC_SPI_READ_STATUS_FIFO_DEPTH7_BIT		0x4000    
#define ASIC_SPI_READ_STATUS_FIFO_DEPTH6_BIT		0x2000  
#define ASIC_SPI_READ_STATUS_FIFO_DEPTH5_BIT		0x1000 
#define ASIC_SPI_READ_STATUS_FIFO_DEPTH4_BIT		0x0800 
#define ASIC_SPI_READ_STATUS_FIFO_DEPTH3_BIT		0x0400 
#define ASIC_SPI_READ_STATUS_FIFO_DEPTH2_BIT		0x0200 
#define ASIC_SPI_READ_STATUS_FIFO_DEPTH1_BIT		0x0100 
#define ASIC_SPI_READ_STATUS_DONE_BIT				0x0001 
#define ASIC_SPI_READ_STATUS_BUSY_BIT				0x0002

// Write Control Register
#define ASIC_SPI_WRITE_CONTROL_EXTERNAL_CLOCK_BIT	0x8000 
#define ASIC_SPI_WRITE_CONTROL_DIV2_BIT				0x4000 
#define ASIC_SPI_WRITE_CONTROL_DIV4_BIT				0x2000 
#define ASIC_SPI_WRITE_WRITE_REGISTERS_VALID_BIT	0x0800 
#define ASIC_SPI_WRITE_WRITE_RESET_BIT				0x1000 

#define ASIC_SPI_WRITE_READ_REGISTERS_DONE_BIT		0x0400 
#define ASIC_SPI_WRITE_OSC_CTRL6_BIT				0x0020 
#define ASIC_SPI_WRITE_OSC_CTRL5_BIT				0x0010 
#define ASIC_SPI_WRITE_OSC_CTRL4_BIT				0x0008 
#define ASIC_SPI_WRITE_OSC_CTRL3_BIT				0x0004 
#define ASIC_SPI_WRITE_OSC_CTRL2_BIT				0x0002 
#define ASIC_SPI_WRITE_OSC_CTRL1_BIT				0x0001 

#define ASIC_SPI_WRITE_TEST_REGISTER				0x0001  // Address
#define ASIC_SPI_READ_TEST_REGISTER					0x0001  // Address

#define ASIC_CHIP_STATUS_DONE		0
#define ASIC_CHIP_STATUS_WORKING	1

void    init_ASIC(void);

// Maximum 32 nonces supported,   //MAX may define, change by JJ

__CHIP_WORKING_STATE ASIC_get_job_status(u8 cChip);

__CHIP_WORKING_STATE ASIC_get_job_status_from_engine(u8 cChip, u16 wEngine);
void		 ASIC_job_issue(u8 cChip);
void 		ASIC_job_issue_to_specified_engine(u8  iChip, 
														u16  iEngine,
														char  bLoadStaticData,
														u16  bIgniteEngine,
														u32 _LowRange,
														u32 _HighRange);

												
void		 ASIC_job_start_processing(char iChip, u16 iEngine, char bForcedStart);
int  		 ASIC_get_chip_count(void);
int			 ASIC_get_chip_processor_count(char iChip);
int  		 ASIC_get_processor_count(void);
char		 ASIC_has_engine_finished_processing(char iChip, u16 iEngine);
char 		ASIC_diagnose_processor(u8 iChip, u16 iEngine);

void		 ASIC_run_scattered_diagnostics(void);
void		 ASIC_run_heavy_diagnostics(void);
void		 ASIC_find_nonce_designated_engine(unsigned int iNonce, unsigned char *iChip, u16 *iEngine);
void		 ASIC_calculate_engines_nonce_range(void);
u32 		ASIC_tune_chip_to_frequency(u8 cChip, u16 wEngine, char bOnlyReturnOperatingFrequency);
int			 ASIC_are_all_engines_done(unsigned int iChip);
void		 ASIC_reset_engine(char iChip, u16 iEngine);
int			 ASIC_does_chip_exist(unsigned int iChipIndex);
int			 ASIC_is_processing(void);
int			 ASIC_is_chip_processing(char iChip); // Chip based function
int			 ASIC_is_engine_processing(char iChip, u16 iEngine); // Engine based function
void		 ASIC_set_clock_mask(char iChip, unsigned int iClockMaskValue);
void		 ASIC_Bootup_Chips(void);
void		 ASIC_ReadComplete(char iChip, u16 iEngine);
void		 ASIC_WriteComplete(char iChip, u16 iEngine);
int			 ASIC_GetFrequencyFactor(void);
void 		ASIC_SetFrequencyFactor(u8 cChip, u16 wFreqFactor);

//void		 __ASIC_WriteEngine(char iChip, char iEngine, unsigned int iAddress, unsigned int iData16Bit);
u16 		__ASIC_WriteEngine(u8 iChip, u16 iEngine, u8 iAddress, u16 iData16Bit);

u16 __ASIC_ReadEngine (u8 iChip, u16 iEngine, u8 iAddress);
u16 CHIP_EXISTS(u8 x);
u16 IS_PROCESSOR_OK(u8 xchip, u16 yengine);

void ASIC_LowLevel_Init(void);
void  MACRO__ASIC_WriteEngineExpress(u8 x, u16 y, u8 z, u16 d);
u16 __ASIC_WriteEngine_NoCs(u8 iChip, u16 iEngine, u8 iAddress, u16 iData16Bit);
u16 __ASIC_ReadEngine_NoCs(u8 iChip, u16 iEngine, u8 iAddress);







#endif /* ASIC_ENGINE_H_ */


