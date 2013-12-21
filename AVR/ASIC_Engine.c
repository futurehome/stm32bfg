/*
 * ASIC_Engine.c
 *
 * Created: 20/11/2012 23:17:15
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */ 
#include "stm32f10x.h"
#include "ASIC_Engine.h"
#include "std_defs.h"
#include "Generic_Module.h"
#include "AVR32_OptimizedTemplates.h"
#include "AVR32X/AVR32_Module.h"
#include "bf_simu_platform.h"
#include "bf_general.h"
#include "bf_peripheral_timer.h"
#include "HostInteractionProtocols.h"
#include "bf_peripheral_uart.h"
#include <stdio.h>
#include <string.h>



//#include "avr32/io.h"

extern SOFT_TIMER SoftTimer1;
extern SOFT_TIMER ChipSoftTimer[TOTAL_CHIPS_INSTALLED];
extern __CHIP_PROCESSING_STATUS ChipMiningStatus[TOTAL_CHIPS_INSTALLED];
extern unsigned char __aux_CharMap[];

u16 __chip_existence_map[TOTAL_CHIPS_INSTALLED]; // Bit 0 to Bit 16 in each word says if the engine is OK or not...
unsigned int __internal_global_iChipCount;
unsigned char GLOBAL_ENGINE_PROCESSING_STATUS[TOTAL_CHIPS_INSTALLED][16]; // If 1, the engine is processing
unsigned char GLOBAL_ENGINE_PROCESSING_FAILURE_SCORES[TOTAL_CHIPS_INSTALLED][16]; // If this number reaches 5 failures, we'll decommission the engine
unsigned int  GLOBAL_ENGINE_PROCESSING_START_TIMESTAMP[TOTAL_CHIPS_INSTALLED][16]; // When did this engine start processing?
unsigned int GLOBAL_TotalEnginesDetectedOnStartup;
unsigned int GLOBAL_CHIP_PROCESSOR_ENGINE_LOWBOUND[TOTAL_CHIPS_INSTALLED][16]; // Calculated on initialization
unsigned int GLOBAL_CHIP_PROCESSOR_ENGINE_HIGHBOUND[TOTAL_CHIPS_INSTALLED][16]; // Calculated on initialization
unsigned int  GLOBAL_ENGINE_MAXIMUM_OPERATING_TIME[TOTAL_CHIPS_INSTALLED][16];
unsigned short GLOBAL_ChipActivityLEDCounter[TOTAL_CHIPS_INSTALLED];
unsigned int GLOBAL_LastJobIssueToAllEngines;
unsigned int GLOBAL_LastJobIssuedToChip[TOTAL_CHIPS_INSTALLED];
unsigned int GLOBAL_LAST_ASIC_IS_PROCESSING_LATENCY;




//static   unsigned int __ActualRegister0Value = (0);
  static   unsigned int __ActualRegister0Value = (1<<13);
//static   unsigned int __ActualRegister0Value = (1);

/*
// Midstate for SHA2-Core, this is static and must be hard-coded in the chip
static const unsigned int STATIC_H1[8] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

static const unsigned int STATIC_W0[64] = {
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x80000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000280,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000
};*/
/*
const job_packet Jb1 = {{0xE6,0x18,0x6E,0xD1,0xF6,0x67,0xA5,0xBD,0xEC,0x43,0x10,0x8B,0x9C,0xD1,0x14,0x43,0x7C,0x86,0x6A,0x57,0x44,0xAC,0x68,0x50,0x68,0xA8,0x09,0x6A,0xF2,0xF4,0x3F,0x71},
							{0x4A,0x85,0xCD,0x2A,0x51,0xA2,0x7F,0x59,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb1NonceCount = 5;
	const unsigned int Jb1Nonces[] = {0x10A0526B,0x1A9F392E,0x26B36CE6,0x5DE1CD41,0xB7704C23};

	const job_packet Jb2 = {{0xE6,0x2D,0xC7,0x55,0x57,0x4D,0x03,0x57,0x4A,0x14,0x86,0x56,0x07,0x31,0xFE,0x5D,0x99,0x87,0x91,0xD2,0x07,0x1B,0xB7,0x8B,0xB4,0x94,0xB8,0x3C,0x3B,0x2F,0x97,0x7F},
							{0x28,0xBA,0xA2,0x08,0x51,0xA4,0x31,0x29,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb2NonceCount = 6;
	const unsigned int Jb2Nonces[] = {0x29F492E6,0x706F72ED,0x9F85AAB7,0xB66A65AA,0xD32B8A93,0xE37CFBB7};

	const job_packet Jb3 = {{0xA0,0x20,0x5F,0xB6,0x89,0x4F,0x5C,0xB0,0x91,0x63,0x8F,0x9B,0x8D,0x9E,0x1C,0xBC,0x48,0x6B,0x9C,0xF9,0xB9,0x4E,0xE9,0x7A,0x1B,0x79,0xEA,0xD4,0x5D,0x3B,0x54,0xF0},
							{0x30,0xC7,0x08,0xE1,0x51,0xA0,0x91,0x42,0x1A,0x01,0x7F,0xE9},0xAA};
	const unsigned int Jb3NonceCount = 4;
	const unsigned int Jb3Nonces[] = {0xA095FE06,0xBBA7E7DE,0xBB2A7C78,0xCA304C5E};

	const job_packet Jb4 = {{0x46,0x8A,0x94,0x48,0x75,0x3F,0x0B,0x85,0xA9,0x3F,0x24,0x59,0x42,0xA5,0x3F,0x9A,0x42,0x49,0x42,0x36,0xB9,0x81,0xC2,0xAC,0xB4,0xFC,0xF9,0xAD,0xA7,0x20,0x52,0xDA},
							{0x04,0xEA,0x8B,0xEC,0x51,0xA1,0x2D,0x4B,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb4NonceCount = 6;
	const unsigned int Jb4Nonces[] = {0x25024301,0x602A9D62,0x767F2311,0x83312A92,0xB050BF88,0xD1BAC2E8};

	const job_packet Jb5 = {{0x03,0xDB,0x99,0x04,0x08,0xED,0xE6,0x91,0xFC,0xA0,0xEE,0xFF,0x4C,0x4D,0xFF,0x85,0x93,0x4A,0x43,0x6F,0xF2,0xEA,0xF3,0x9A,0x70,0xC5,0x8D,0xA7,0x8F,0x46,0x09,0x5D},
							{0x7E,0xD9,0x1F,0xA4,0x51,0xA2,0xB4,0x66,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb5NonceCount = 5;
	const unsigned int Jb5Nonces[] = {0x6B89867B,0x7F693FDB,0x80B1AC61,0xC0887DE1,0xFF47C49B};

	const job_packet Jb6 = {{0x23,0x65,0x7B,0x08,0x5A,0x96,0x95,0x61,0xCC,0x39,0x3B,0x98,0x0C,0x06,0x4A,0xCA,0xA7,0xB0,0x02,0x00,0xF5,0x96,0x16,0xFD,0x40,0x2B,0xA9,0x05,0xE6,0xE8,0x13,0xF6},
							{0x96,0xFF,0x2A,0xA8,0x51,0xA6,0x24,0x99,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb6NonceCount = 8;
	const unsigned int Jb6Nonces[] = {0x12AAE744,0x1F8F2C96,0x31A926F1,0x5621B3BE,0x68BE97F3,0x8E08EB9F,0xCE2ACC32,0xDFD69EFF};

	const job_packet Jb7 = {{0x33,0xFB,0x46,0xDC,0x61,0x2A,0x7A,0x23,0xF0,0xA2,0x2D,0x63,0x31,0x54,0x21,0xDC,0xAE,0x86,0xFE,0xC3,0x88,0xC1,0x9C,0x8C,0x20,0x18,0x10,0x68,0xFC,0x95,0x3F,0xF7},
							{0xEF,0xAF,0xBA,0xC3,0x51,0xA3,0x6F,0x32,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb7NonceCount = 8;
	const unsigned int Jb7Nonces[] = {0x045ABCED,0x50460F87,0x6C4054E0,0x7C04C5EA,0x82D56423,0xA1C4B2A0,0xECF081A3,0xF803DF88};

	const job_packet Jb8 = {{0x0C,0x3F,0xBA,0x61,0x39,0xE6,0xA5,0x2A,0xF6,0xDC,0xFF,0x30,0xFA,0x60,0x0B,0x91,0x6F,0xD8,0x94,0x5F,0xE5,0x10,0xBE,0x83,0xAC,0x55,0xC8,0xCB,0x42,0xD9,0x35,0xFD},
							{0xC6,0xD6,0x6C,0xF1,0x51,0xA0,0xF6,0x74,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb8NonceCount = 8;
	const unsigned int Jb8Nonces[] = {0x1697AA6E,0x37DB36F3,0x483C418F,0x4E11BB8C,0x8AC19761,0xD58C3BAD,0xDEDB2846,0xEDA9ECC3};

	const job_packet Jb9 = {{0x1E,0xB2,0xCA,0xBD,0xCF,0x29,0x4E,0x9D,0x74,0x3C,0x10,0x68,0x5D,0x02,0x42,0x27,0xA8,0x41,0xAA,0x1B,0x7C,0x1E,0x62,0xEF,0x14,0x05,0xB5,0x7A,0x68,0x78,0x3F,0xF9},
							{0x48,0xD7,0xF3,0xBC,0x51,0xA0,0xAF,0x8A,0x1A,0x01,0x7F,0xE9},0xAA};
	const unsigned int Jb9NonceCount = 6;
	const unsigned int Jb9Nonces[] = {0x1B6B3897,0x202FB51B,0x24600E29,0xB3ABA295,0xD62D4CCF,0xE1629DEC};

	const job_packet Jb10 = {{0xE1,0xD3,0x5E,0x68,0x0D,0x61,0x39,0x77,0x7C,0xA5,0x3D,0xAE,0x12,0xC7,0xF8,0x85,0xBF,0xA9,0xD8,0x47,0x1C,0x74,0x98,0xB3,0xC2,0xA3,0x98,0xE3,0x92,0xB6,0x00,0xDB},
							{0x39,0x48,0xC6,0x4C,0x51,0xA2,0xC3,0x57,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb10NonceCount = 5;
	const unsigned int Jb10Nonces[] = {0x0907982C,0x73D1466F,0xAFF19311,0xC9D5DEAB,0xD209C128};

	const job_packet Jb11 = {{0xFA,0xC9,0x5B,0xF2,0xDB,0x48,0x33,0x39,0x2B,0x9C,0x90,0xCC,0x24,0xE0,0x14,0x64,0xE9,0xE8,0x8B,0x7A,0x4F,0xC6,0xA3,0xF6,0x33,0xA0,0xBF,0x14,0x80,0x35,0x24,0x01},
							{0x45,0xCF,0x62,0xA9,0x51,0x9E,0x2F,0xA6,0x1A,0x01,0x7F,0xE9},0xAA};
	const unsigned int Jb11NonceCount = 8;
	const unsigned int Jb11Nonces[] = {0x02F9337D,0x1EBC244A,0x71876D9F,0x7DC42DCE,0x907C472C,0x9E8A9241,0xAE8DFC00,0xC67BECD3};

	const job_packet Jb12 = {{0x9A,0xEB,0xC4,0x19,0x4A,0xB4,0xDC,0x6C,0x0E,0x7A,0xCE,0x73,0xE6,0x34,0x76,0x86,0xC6,0xAB,0xC4,0xB4,0x32,0xA6,0x14,0xC8,0x74,0xA9,0x9A,0x26,0x88,0xCA,0x60,0xE3},
							{0x99,0xFD,0xD3,0x2E,0x51,0xA1,0x39,0xDE,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb12NonceCount = 8;
	const unsigned int Jb12Nonces[] = {0x15ECCABA,0x1CB1057D,0x2BB40569,0x38A25207,0x638A68AF,0x66E1E0CB,0xBD3CEFBE,0xC47C5885};

	const job_packet Jb13 = {{0xD4,0xC8,0x39,0xF8,0x44,0xFF,0x65,0xFE,0x45,0x27,0x35,0x63,0xCD,0x32,0xC2,0x08,0x4F,0xEC,0x43,0xEA,0x88,0xEE,0x2C,0xB3,0xAF,0x92,0xC4,0xBA,0xAE,0x4E,0x47,0x75},
							{0xB5,0xEF,0x21,0xB3,0x51,0xAA,0x1C,0x8F,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb13NonceCount = 8;
	const unsigned int Jb13Nonces[] = {0x275BA4EA,0x2EF13608,0x4FF199A1,0x5886FA5D,0x5E9F0201,0x77C8B512,0xE0E0A2E7,0xFBA47DE9};

	const job_packet Jb14 = {{0x5D,0x94,0x7F,0x1C,0x9B,0x38,0x69,0x5A,0x6F,0x0D,0x23,0x9F,0x84,0xFD,0x7B,0x52,0x74,0x80,0x7B,0xF6,0x71,0xD5,0x9D,0xF7,0x19,0x40,0xEB,0xDF,0x20,0x87,0x04,0xF1},
							{0xFE,0xB9,0x3B,0x02,0x51,0xA0,0x68,0xBE,0x1A,0x01,0x7F,0xE9},0xAA};
	const unsigned int Jb14NonceCount = 2;
	const unsigned int Jb14Nonces[] = {0x57B7E9B2,0x57A705D6};

	const job_packet Jb15 = {{0x31,0x9C,0x0F,0x0B,0xDD,0x1D,0xD2,0x17,0x68,0x66,0xF5,0x49,0x8E,0x20,0xEC,0xAC,0xC5,0x6E,0x60,0x58,0xF3,0x37,0xF0,0x6C,0x57,0x6A,0x8D,0x78,0xF0,0x77,0x0C,0x8D},
							{0xE9,0x75,0xFD,0x27,0x51,0xA1,0xE2,0xCD,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb15NonceCount = 6;
	const unsigned int Jb15Nonces[] = {0x0D9BE324,0x14726809,0x21AA9298,0x5A727A9D,0x678B21B1,0x95977F9B};

	const job_packet Jb16 = {{0x35,0x3D,0x6F,0x4A,0x72,0x2F,0xB6,0x8C,0x9B,0x90,0x0A,0xAD,0x18,0xCA,0x1D,0x5F,0xC0,0xCC,0x05,0x2D,0x08,0xD4,0xDE,0x32,0x44,0x86,0x7D,0x01,0x21,0x0F,0xAC,0xA4},
							{0xD8,0x03,0x0E,0x41,0x51,0xA0,0x96,0x2F,0x1A,0x01,0x7F,0xE9},0xAA};
	const unsigned int Jb16NonceCount = 4;
	const unsigned int Jb16Nonces[] = {0x35970F0F,0x9429D751,0xC2958995,0xC1450CB2};

	const job_packet Jb17 = {{0x5B,0x1C,0xA4,0x3B,0x12,0xEA,0xED,0x32,0xA5,0x56,0x0D,0xE5,0x92,0xEF,0xF3,0xB1,0xD5,0x84,0x36,0xA1,0x53,0x12,0x17,0x36,0xEF,0xD7,0x9E,0xE8,0x71,0x2A,0xD5,0xA8},
							{0x73,0x10,0x13,0x33,0x51,0xA2,0xE7,0xE5,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb17NonceCount = 3;
	const unsigned int Jb17Nonces[] = {0x3A73FF2A,0x3A73CEA0,0x4328EF3F};

	const job_packet Jb18 = {{0xE9,0x64,0x98,0x36,0x42,0x9A,0xDF,0xA1,0xE4,0xB0,0x75,0x1A,0x62,0xB6,0x13,0x47,0xB8,0xC7,0xC4,0x09,0x9C,0x55,0xF3,0x98,0x96,0x50,0x4E,0x5C,0x0D,0xEB,0x58,0x4A},
							{0x23,0x4E,0x53,0x8D,0x51,0xA4,0x9E,0x90,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb18NonceCount = 7;
	const unsigned int Jb18Nonces[] = {0x18ADA8CF,0x40D88BF1,0x4AB848F8,0x8513A070,0x912AB988,0x99342A54,0xEFE784A9};

	const job_packet Jb19 = {{0x2C,0x78,0xB6,0xCB,0x6D,0x16,0xAA,0x73,0xFA,0xB0,0x6C,0x7B,0xB1,0x97,0x43,0x6B,0xCB,0x72,0xEF,0x83,0xB4,0x5F,0x68,0x2E,0x5A,0x12,0x9D,0x80,0x01,0x2D,0x48,0x9A},
							{0x7A,0xE8,0x22,0x9F,0x51,0xA1,0x4F,0x02,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb19NonceCount = 6;
	const unsigned int Jb19Nonces[] = {0x3789C609,0x7EC6F3C6,0x89094A50,0x9A45D481,0xDBC8ED45,0xEBCE64D8};

	const job_packet Jb20 = {{0x0A,0x6F,0x6C,0xBB,0x9B,0x57,0x79,0x66,0xF3,0x6F,0x96,0xA7,0xC8,0x25,0x9D,0xA9,0xF0,0x04,0xD6,0x20,0x86,0x6F,0xD0,0x72,0x2E,0xBB,0x67,0xD5,0xF5,0xBB,0x1F,0xEB},
							{0xD9,0xED,0xD2,0xEC,0x51,0xA0,0x6E,0x06,0x1A,0x01,0x7F,0xE9},0xAA};
	const unsigned int Jb20NonceCount = 8;
	const unsigned int Jb20Nonces[] = {0x467070CB,0x00700FCA,0x47E30E0C,0x5B12AB07,0x75E31B9C,0xA536947D,0xA52FA951,0xCBB74F51};

	const job_packet Jb21 = {{0x55,0xEA,0xA9,0xC3,0x49,0x52,0x60,0x03,0x6A,0xAC,0x7A,0xA9,0xD4,0x66,0x03,0x98,0x6D,0x39,0x01,0x1B,0x74,0xBA,0x34,0xA4,0x83,0xF3,0x52,0xBA,0x1E,0x08,0xB9,0xBB},
							{0x80,0x70,0x0F,0x35,0x51,0xA0,0x97,0x79,0x1A,0x01,0x7F,0xE9},0xAA};
	const unsigned int Jb21NonceCount = 8;
	const unsigned int Jb21Nonces[] = {0x1D43B6D4,0x39BEB0F1,0x3DDBF217,0x42B37E62,0x72B9FA58,0xA76CAB7D,0xACFBAE3C,0xB2FB3698};

	const job_packet Jb22 = {{0x7B,0x85,0x8D,0x68,0xEC,0xB2,0x85,0xDA,0x23,0x55,0x26,0x90,0xFA,0xBC,0x4B,0x20,0x0D,0x9B,0xC2,0x6A,0x68,0x62,0x61,0x6D,0x0E,0xD1,0x19,0x24,0x7B,0xE0,0xD9,0x1B},
							{0x47,0x83,0xE1,0x71,0x51,0xA8,0x56,0xB5,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb22NonceCount = 4;
	const unsigned int Jb22Nonces[] = {0x037B05BA,0x05BC1F12,0x5516BC7C,0x93FFBE74};

	const job_packet Jb23 = {{0x2B,0xF9,0x0D,0x36,0x98,0xEE,0x6D,0xED,0x7C,0x80,0x40,0xA0,0xBF,0x77,0x47,0x5E,0xE1,0xC7,0x44,0xC2,0x5D,0x98,0x2E,0x1A,0xB6,0x28,0x48,0x90,0x16,0xBB,0xA3,0x0D},
							{0xF4,0x70,0x05,0xDE,0x51,0xA6,0x07,0x1E,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb23NonceCount = 6;
	const unsigned int Jb23Nonces[] = {0x0CC84A69,0x629E244C,0x796CB8E8,0x92246910,0xBC6F22DE,0xBFFD6937};

	const job_packet Jb24 = {{0x31,0x71,0x1E,0x0F,0xBD,0xAC,0x7B,0x6D,0xFC,0x68,0x33,0x1E,0x13,0x53,0xFC,0xFE,0x7A,0x2E,0x9B,0xDF,0xFB,0xAA,0x9C,0xDF,0xB3,0xC4,0x90,0x2C,0xB6,0x12,0x4C,0x9D},
							{0x78,0x37,0x18,0x29,0x51,0xA5,0x02,0xB5,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb24NonceCount = 7;
	const unsigned int Jb24Nonces[] = {0x0A8416F2,0x17F08781,0x4DFF831F,0xA486221E,0xB8A41E06,0xDDA47A07,0xE4CC9891};

	const job_packet Jb25 = {{0xFE,0x54,0xB4,0x19,0xF8,0x60,0xFB,0x17,0x95,0xC3,0x2F,0x7E,0x21,0xCE,0x3C,0xC4,0x4B,0xA7,0x55,0xB7,0x8A,0x14,0x85,0x85,0x4B,0xA1,0x0B,0xE4,0x36,0x05,0x3F,0x6B},
							{0x1A,0x61,0xCE,0xE9,0x51,0xA3,0xEA,0x41,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb25NonceCount = 8;
	const unsigned int Jb25Nonces[] = {0x0B9F2392,0x2C33E106,0x3BEEC981,0x4598DC4C,0x69E3ACFB,0x8F03F631,0xA60284E2,0xC58F3975};

	const job_packet Jb26 = {{0x49,0xA3,0xE8,0x94,0x67,0xC1,0xBC,0x09,0x27,0x32,0x8B,0xE3,0x3F,0x0F,0xE9,0xAD,0xFC,0x1F,0x15,0x39,0xF1,0x61,0xB1,0xD9,0x20,0x37,0x02,0x08,0x1B,0x48,0x7A,0x66},
							{0xC3,0x70,0x56,0x66,0x51,0xA2,0x0C,0xB0,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb26NonceCount = 4;
	const unsigned int Jb26Nonces[] = {0x3F318DF5,0x8892958F,0x86972264,0xD7A8021D};

	const job_packet Jb27 = {{0xE0,0xE4,0xF6,0xB3,0x95,0x52,0x5F,0x4E,0x52,0x21,0x4D,0x2B,0xC2,0xC5,0x3D,0x9A,0xA8,0x62,0x3E,0xBC,0x78,0x0C,0xDE,0xD3,0x95,0x60,0x44,0x8F,0x7B,0xF7,0x00,0x6F},
							{0x67,0x33,0xFA,0x48,0x51,0xA6,0xD3,0x55,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb27NonceCount = 6;
	const unsigned int Jb27Nonces[] = {0x133BDCF1,0x6ABA8BE0,0xAD2EC052,0xD0D8179F,0xF5DF20D4,0xFC417263};

	const job_packet Jb28 = {{0xB5,0xC5,0xAC,0x44,0xB1,0x67,0xB1,0xE9,0x22,0xAE,0xF4,0x64,0xA1,0x88,0xB5,0x53,0xB0,0x54,0x2F,0xFE,0xE8,0x7F,0x55,0x4E,0xCF,0x00,0xEE,0xBC,0x51,0x01,0x04,0xAF},
							{0x26,0x9F,0x5C,0xAE,0x51,0xAA,0x79,0xA5,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb28NonceCount = 5;
	const unsigned int Jb28Nonces[] = {0x2231A9FF,0xA24EB87A,0xBA597D4F,0xCDFC8B94,0xE5180111};

	const job_packet Jb29 = {{0x17,0x4C,0x9D,0x7A,0x59,0x28,0x8A,0x92,0xB8,0x44,0xC7,0x83,0x3D,0x22,0xBA,0xF5,0x55,0x9B,0x4C,0x38,0x77,0x09,0x67,0x30,0xDF,0xB5,0x89,0x78,0xE0,0xE7,0x15,0xFF},
							 {0xAD,0x33,0x85,0xF0,0x51,0xA9,0xCD,0x0F,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb29NonceCount = 5;
	const unsigned int Jb29Nonces[] = {0x2DA07DC3,0x5F35B344,0x98EB134D,0xCC68B19B,0xF3A02953};

	const job_packet Jb30 = {{0xD0,0xF9,0x2F,0x82,0x17,0xFA,0xDD,0xC6,0x1D,0x19,0x5D,0xB8,0x91,0x86,0x2B,0xB8,0x78,0xA8,0xDC,0x7F,0x87,0xD9,0x2F,0xFB,0xCF,0x0A,0x4F,0x04,0x86,0x0B,0xDD,0x18},
							 {0xB9,0x1E,0xB9,0x2C,0x51,0xA0,0x6F,0x19,0x1A,0x01,0x7F,0xE9},0xAA};
	const unsigned int Jb30NonceCount = 7;
	const unsigned int Jb30Nonces[] = {0x2A06707A,0x4CD53DA0,0x61843CC3,0x748B2214,0xD4270463,0xE2E7B310,0xE75E006D};

	const job_packet Jb31 = {{0xC3,0x1A,0xD3,0x14,0x0B,0x0C,0x98,0x4E,0x25,0x1B,0xF4,0xFF,0xF0,0xD8,0xDA,0xEA,0x9F,0x47,0xC0,0xEC,0xAE,0x09,0xD4,0x3B,0xE4,0x61,0x2F,0x7C,0x70,0x5C,0x2F,0x3C},
							 {0x99,0x9F,0x61,0x8A,0x51,0xA7,0x00,0xF9,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb31NonceCount = 7;
	const unsigned int Jb31Nonces[] = {0x3343F3DC,0x3251AAFD,0x547CDA73,0x96FC456B,0x9C1AB3EC,0xBEE20E37,0xFEED7C9F};

	const job_packet Jb32 = {{0x75,0x03,0x66,0xE7,0xE2,0xA0,0x6B,0x9F,0x66,0xA1,0x5C,0x37,0xFB,0x0D,0x33,0x77,0x96,0x00,0xA1,0xFF,0xC5,0x81,0xF9,0xF1,0x07,0xEC,0x4C,0xC6,0xB6,0xA2,0x9F,0x8E},
							 {0xF9,0x6B,0x5D,0x9F,0x51,0xA6,0x34,0xE3,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb32NonceCount = 6;
	const unsigned int Jb32Nonces[] = {0x3C3A6532,0x4BFF7A46,0x7B0386E6,0x9BA67DF1,0xA971ABA8,0xFA12560C};

	const job_packet Jb33 = {{0x93,0xF5,0xA2,0x2E,0x19,0x2F,0x8C,0xBE,0x2C,0xBF,0x93,0x6B,0xA7,0x0D,0x42,0x0C,0xEB,0x83,0x5E,0xA1,0x0E,0xF0,0x0A,0xC0,0x13,0xAC,0xD6,0xF9,0x55,0x80,0x56,0xD3},
							 {0xD1,0x80,0x61,0x1D,0x51,0xA0,0x75,0x31,0x1A,0x01,0x7F,0xE9},0xAA};
	const unsigned int Jb33NonceCount = 2;
	const unsigned int Jb33Nonces[] = {0x0880D2CC,0xC38A02AA};

	const job_packet Jb34 = {{0x1E,0xD7,0x3B,0xF6,0xE6,0xB5,0xBA,0x30,0x63,0x62,0xCE,0xFA,0x5B,0x4B,0x30,0x0D,0x9F,0xFB,0x7C,0x7B,0xBE,0xF9,0x68,0xF7,0x5D,0x43,0x99,0x34,0xF5,0xA6,0x81,0x84},
							 {0xC3,0xB9,0xA3,0x29,0x51,0xA6,0xAD,0x73,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb34NonceCount = 4;
	const unsigned int Jb34Nonces[] = {0x517CAFC3,0x6EAAE01B,0x9D234679,0xA3459218};

	const job_packet Jb35 = {{0x9D,0xF7,0x09,0xED,0xC6,0x8A,0x7C,0x5E,0x5A,0xA8,0x80,0x33,0x9E,0x82,0x33,0x78,0x64,0x23,0xD1,0x7F,0xB7,0x98,0x1D,0x57,0x66,0x71,0x85,0x42,0x08,0xE6,0xF2,0x3A},
							 {0x06,0xA6,0xF0,0xD5,0x51,0xA2,0xF3,0xEF,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb35NonceCount = 6;
	const unsigned int Jb35Nonces[] = {0x0E5A0461,0x44373376,0x8CAB5F09,0xD8168C99,0xE9BF870E,0xEE4F8827};

	const job_packet Jb36 = {{0x9B,0x64,0x44,0x93,0x38,0x1D,0x93,0xC6,0x40,0x49,0x8A,0x5C,0xCC,0x91,0x84,0x0B,0xA9,0x9B,0x32,0xE4,0x1D,0xE8,0xEC,0xDE,0x16,0xD6,0xC1,0xCE,0xD4,0x48,0x24,0x14},
							 {0xE9,0x84,0x7C,0x6E,0x51,0xA8,0x41,0x6B,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb36NonceCount = 7;
	const unsigned int Jb36Nonces[] = {0x280AD742,0x4148652D,0x6F16810D,0x8E0E7749,0xA8E8EF8F,0xA891C7B8,0xD96D0260};

	const job_packet Jb37 = {{0xCB,0x47,0x43,0x32,0x45,0xBA,0x4B,0x30,0x74,0xE2,0x5B,0xD4,0xA1,0xC0,0x29,0x14,0xFB,0xC7,0x32,0x3B,0x73,0xDD,0xA4,0x9A,0x8E,0x63,0x5D,0x96,0x93,0x0E,0xBC,0xC1},
							 {0x12,0x51,0x01,0xBA,0x51,0x9D,0x83,0x52,0x1A,0x01,0x7F,0xE9},0xAA};
	const unsigned int Jb37NonceCount = 5;
	const unsigned int Jb37Nonces[] = {0x6DC61A6D,0x87CF2C61,0x8DA78BCA,0xE80F2B72,0xF07D0DEB};

	const job_packet Jb38 = {{0x0C,0x27,0x9D,0x68,0x8D,0x10,0x05,0xA5,0x08,0xD8,0xD9,0xF0,0xEE,0xAF,0x6A,0x42,0x1E,0x94,0x51,0x7F,0x4F,0x49,0x0F,0xCB,0xB4,0x2D,0xF8,0xAC,0xCA,0xD9,0x5D,0x07},
							 {0xCC,0xA9,0xD5,0xF8,0x51,0xAB,0xD6,0xD3,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb38NonceCount = 8;
	const unsigned int Jb38Nonces[] = {0x817C5707,0xAA288F82,0xB1DDF664,0xB4BC1112,0xC7C02047,0xCF9FFF9F,0xF2ED0F65,0xF1C59B14};

	const job_packet Jb39 = {{0x1F,0x22,0x58,0x9E,0xCB,0x05,0xA3,0x65,0xA0,0xA4,0x02,0x6F,0xD3,0x87,0x5B,0x49,0xE4,0x77,0x9C,0x49,0xE0,0xF8,0x9C,0xE6,0xF0,0x7F,0xF9,0x52,0x2F,0x2E,0x37,0x31},
							 {0xC7,0x1D,0xBA,0xAC,0x51,0xAE,0xCC,0x65,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb39NonceCount = 7;
	const unsigned int Jb39Nonces[] = {0x0176D001,0x07D7CD9D,0x23D91426,0x36EF4845,0x65AABF30,0xB9E2E226,0xF733484B};

	const job_packet Jb40 = {{0x65,0x8E,0x52,0xDC,0xA2,0x8E,0xC3,0x5F,0xD7,0x08,0x4E,0xA3,0xA5,0x7C,0x38,0xB0,0x03,0xEA,0xF8,0x2B,0x47,0x7E,0x55,0xBE,0xEF,0xFF,0xC4,0x4B,0x0E,0xF5,0x0C,0x80},
							 {0x63,0xFD,0x49,0xF2,0x51,0xA2,0x3A,0x1A,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb40NonceCount = 5;
	const unsigned int Jb40Nonces[] = {0x11D9BB2C,0x3074097E,0x49EEAA5E,0x598A8395,0xFD2742EF};

	const job_packet Jb41 = {{0xDB,0x02,0x80,0xCA,0x58,0x17,0x46,0x3E,0xAA,0x05,0x87,0x69,0xC3,0xB3,0x8F,0x4E,0x2C,0xB0,0xFA,0x08,0xF3,0x97,0x20,0x0A,0xD1,0x01,0x72,0xCB,0x91,0x3A,0x8D,0xC7},
							 {0xEB,0x6C,0x68,0x80,0x51,0xA1,0xAE,0x5F,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb41NonceCount = 8;
	const unsigned int Jb41Nonces[] = {0x0F83379F,0x3E145360,0x64DD5309,0xC88E0D8E,0xDAFD7BE9,0xF4AD7CFD,0xF6DA3CAB,0xF9B1BC01};

	const job_packet Jb42 = {{0xEF,0x77,0xB1,0x42,0xE0,0xD2,0x14,0xEF,0xBA,0x3C,0x75,0xE1,0xD9,0x2D,0x40,0x18,0x4F,0x93,0xD1,0xBA,0x99,0x90,0xF7,0x04,0xED,0x42,0x5A,0x87,0x7E,0x6F,0x21,0xAF},
							 {0x17,0x11,0xE9,0x85,0x51,0xA0,0xF0,0xEF,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb42NonceCount = 6;
	const unsigned int Jb42Nonces[] = {0x2F31B195,0x53AEDCD6,0x841283A8,0x97835A3E,0xB51E0042,0xDC24E16D};

	const job_packet Jb43 = {{0xFC,0xA7,0x13,0x08,0x92,0x37,0x1C,0x8F,0x6F,0xD1,0xFD,0x67,0x68,0x83,0xAA,0xB5,0x2A,0x20,0x9B,0x0A,0x7C,0xF9,0xD7,0x37,0xB3,0x6B,0x33,0x1F,0x03,0xD7,0x1F,0x36},
							 {0x1B,0x08,0xA9,0x6E,0x51,0xA7,0x2F,0x0E,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb43NonceCount = 4;
	const unsigned int Jb43Nonces[] = {0x0654FDA9,0xE6EB6582,0xEAED6990,0xEAC4E07E};

	const job_packet Jb44 = {{0x7E,0xE5,0xA9,0x0E,0x23,0x58,0x0D,0xE2,0xD3,0x9C,0xAA,0xD8,0xC9,0xDE,0x79,0x35,0x5C,0xDE,0x49,0x70,0x9B,0x91,0x66,0x27,0xE9,0xB5,0xE7,0x6E,0xD0,0x07,0x9B,0x5E},
							 {0x07,0x16,0x51,0xE7,0x51,0xA1,0x7F,0x7F,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb44NonceCount = 5;
	const unsigned int Jb44Nonces[] = {0x1946E8BA,0x296E17F6,0x34F3B5D8,0x78D0E8A7,0x8BEBF64A};

	const job_packet Jb45 = {{0x86,0x20,0xD3,0x05,0xF7,0x81,0x89,0x6F,0x8F,0x23,0x03,0x47,0xFA,0xE8,0x1C,0xDA,0x32,0xB6,0xD3,0x6D,0x74,0x5D,0x68,0x05,0x94,0x97,0x5A,0x09,0xE7,0xEF,0x34,0xDF},
							 {0x91,0x19,0x0F,0x1A,0x51,0xA4,0xF3,0xAF,0x1A,0x01,0x61,0x64},0xAA};
	const unsigned int Jb45NonceCount = 5;
	const unsigned int Jb45Nonces[] = {0x017E8A19,0x528D4959,0x5C1FCB93,0x7AFEAC51,0xAB6B205B};
*/	
	const char __stat_blockdata[] = {0xAC,0x84,0xF5,0x8B,0x4D,0x59,0xB7,0x4D,0x1B,0x2,0x85,0x52};
	//const char __stat_blockdata[] = {0xAC,0x84,0xF5,0x8B,0x4D,0x59,0xB7,0x4D,0x1B,0x2,0x85,0x52};
	const char __stat_midstate[]	= {0x3C,0x42,0x49,0xA8,0xE1,0xC5,0x45,0x78,0xA5,0x2D,0x83,0xC1,0x1,0xE5,0xC5,0x8E,0xF5,0x2F,0x3,0xD,0xEE,0x2E,0x9D,0x29,0xB6,0x94,0x9A,0xDF,0xA6,0x95,0x97,0xAE};
	//const char __stat_midstate[]	= {0x3C,0x42,0x49,0xA8,0xE1,0xC5,0x45,0x78,0xA5,0x2D,0x83,0xC1,0x1,0xE5,0xC5,0x8E,0xF5,0x2F,0x3,0xD,0xEE,0x2E,0x9D,0x29,0xB6,0x94,0x9A,0xDF,0xA6,0x95,0x97,0xAE};

u32 dwTimeStart;
u32 dwTimeEnd;
u8 cGoodEngineCount[TOTAL_CHIPS_INSTALLED];

//_BF_CHIP_STATUS BfChipStatusTable[TOTAL_CHIPS_INSTALLED];

void ASIC_LowLevel_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*!< BF Done Periph clock enable */
	RCC_APB2PeriphClockCmd(BF_DONE0_GPIO_CLK | BF_DONE1_GPIO_CLK | BF_DONE2_GPIO_CLK |
						 BF_DONE3_GPIO_CLK, ENABLE);

	/*!< Configure DONE0 pins:  */
	GPIO_InitStructure.GPIO_Pin = BF_DONE0_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BF_DONE0_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure DONE1 pins:	*/
	GPIO_InitStructure.GPIO_Pin = BF_DONE1_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BF_DONE1_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure DONE2 pins:	*/
	GPIO_InitStructure.GPIO_Pin = BF_DONE2_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BF_DONE2_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure DONE3 pins:	*/
	GPIO_InitStructure.GPIO_Pin = BF_DONE3_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BF_DONE3_GPIO_PORT, &GPIO_InitStructure);

}



// Midstate for SHA1-Core, this is provided by the MCU (Which is in turn provided by the GetWork API)
void init_ASIC(void)
{
		// Ok now we diagnose the engines
	char iHoveringChip;
	char iHoveringEngine;
	u32 dwDelay;
	u8 cChip;
	u8 cEngine;

	MCU_SC_Initialize();			//BF done signal configered in SPI init subroutine
	
	// Clear ASIC map
	__chip_existence_map[0] = 0;
	__chip_existence_map[1] = 0;
	__chip_existence_map[2] = 0;
	__chip_existence_map[3] = 0;
	__chip_existence_map[4] = 0;
	__chip_existence_map[5] = 0;
	__chip_existence_map[6] = 0;
	__chip_existence_map[7] = 0;
	
	#if defined(__PRODUCT_MODEL_SINGLE__) || defined(__PRODUCT_MODEL_MINIRIG__) 
		__chip_existence_map[8] = 0;
		__chip_existence_map[9] = 0;
		__chip_existence_map[10] = 0;
		__chip_existence_map[11] = 0;
		__chip_existence_map[12] = 0;
		__chip_existence_map[13] = 0;
		__chip_existence_map[14] = 0;
		__chip_existence_map[15] = 0;	
	#endif
	
	// Here we must give 16 clock cycles with CS# inactive, to initialize the ASIC chips
	__MCU_ASIC_Deactivate_CS();
	//__MCU_ASIC_Deactivate_CS(2);		//only 1 spi bus used in Bf_main
	
	// Reset first bank
	__ASIC_WriteEngine_NoCs(0,0,0,0);
	__ASIC_WriteEngine_NoCs(0,0,0,0);
	__ASIC_WriteEngine_NoCs(0,0,0,0);
	__ASIC_WriteEngine_NoCs(0,0,0,0);

	// Reset second bank
	/*
	#if defined(__PRODUCT_MODEL_SINGLE__) || defined(__PRODUCT_MODEL_MINIRIG__) 
		__ASIC_WriteEngine(8,0,0,0);
		__ASIC_WriteEngine(8,0,0,0);
		__ASIC_WriteEngine(8,0,0,0);
		__ASIC_WriteEngine(8,0,0,0);		
	#endif*/
	dwDelay = 0x02ff;
				while((dwDelay--) != 0)
				{
				}
	// Also here we set the Oscillator Control Register
	ASIC_Bootup_Chips();
		
	// Proceed...
	__internal_global_iChipCount = 0; // Reset
	
	__MCU_ASIC_Deactivate_CS();
	//__MCU_ASIC_Activate_CS();
	/*
	__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
	//__ASIC_WriteEngine(3,0, 0, 0x2001);
		__ASIC_WriteEngine(3, 0, 1,0x5AA5);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
	//__ASIC_WriteEngine(3,0, 0, 0x1000);*/
	/*
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 1);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 0);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 0x60);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 0x61);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 0x62);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 0x63);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 0x64);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 0x65);
	__internal_global_iChipCount += __ASIC_ReadEngine(3,0, 0x66);*/
	//	__MCU_ASIC_Deactivate_CS();
	__internal_global_iChipCount = 0;
	ASIC_get_chip_count(); // Also sets the '__chip_existence_map' table in it's first run (and in Recalibrate mode)
	

	
	//for (iHoveringChip = 0; iHoveringChip < TOTAL_CHIPS_INSTALLED; iHoveringChip++)
	for (iHoveringChip = 3; iHoveringChip < 4; iHoveringChip++)
	{
		// Reset WATCHDOG
		WATCHDOG_RESET;
		
		// Does the chip exist at all?
		if (!CHIP_EXISTS(iHoveringChip)) 
		{
			continue;
		}
					
		// Diagnose
		for (iHoveringEngine = 0; iHoveringEngine < 16; iHoveringEngine++)
		{
			// Reset watchdog
			WATCHDOG_RESET;
			
			// Is Engine 0 permitted?
			#if defined(DO_NOT_USE_ENGINE_ZERO)
				if (iHoveringEngine == 0) continue;			
			#endif
			
			// Is processor ok or does chip exist?
			if (!CHIP_EXISTS(iHoveringChip)) continue;
			if (!IS_PROCESSOR_OK(iHoveringChip, iHoveringEngine)) continue;
			
			// Proceed
			#if defined(ASIC_DIAGNOSE_ENGINE_REMAINING_BUSY)
				if (ASIC_diagnose_processor(iHoveringChip, iHoveringEngine) == FALSE)
				{
					__chip_existence_map[iHoveringChip] &= ~(1<<iHoveringEngine);
				}
				else
				{
					__chip_existence_map[iHoveringChip] |= (1<<iHoveringEngine);
				}
			#else
				__chip_existence_map[iHoveringChip] |= (1<<iHoveringEngine);
			#endif				
		}
		
		// Check for Enforced usage!
		#if defined(ENFORCE_USAGE_CHIP_0)
			__chip_existence_map[0] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_1)
			__chip_existence_map[1] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_2)
			__chip_existence_map[2] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_3)
			__chip_existence_map[3] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_4)
			__chip_existence_map[4] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_5)
			__chip_existence_map[5] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_6)
			__chip_existence_map[6] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_7)
			__chip_existence_map[7] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_8)
			__chip_existence_map[8] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_9)
			__chip_existence_map[9] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_10)
			__chip_existence_map[10] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_11)
			__chip_existence_map[11] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_12)
			__chip_existence_map[12] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_13)
			__chip_existence_map[13] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
		
		#if defined(ENFORCE_USAGE_CHIP_14)
			__chip_existence_map[14] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif
																														
		#if defined(ENFORCE_USAGE_CHIP_15)
			__chip_existence_map[15] = 0xFFFE; // ENGINE ZERO NOT USED
		#endif		
		
		// Frequency tune the chips
		#if !defined(__DO_NOT_TUNE_CHIPS_FREQUENCY)
			for (iHoveringEngine = 0; iHoveringEngine < 16; iHoveringEngine++)		
			{
				// Is Engine 0 permitted?
				#if defined(DO_NOT_USE_ENGINE_ZERO)
					if (iHoveringEngine == 0) continue;
				#endif			
			
				// If this processor is bad, continue...
				if (!IS_PROCESSOR_OK(iHoveringChip, iHoveringEngine)) continue;
						
				// Use it to diagnose
				int iDetectedFreq = ASIC_tune_chip_to_frequency(iHoveringChip, iHoveringEngine, FALSE);
				GLOBAL_CHIP_FREQUENCY_INFO[iHoveringChip] = (iDetectedFreq / 1000000);
				break; // We break after here...
			}
		#else
			// Since job-distibution uses this information, if we're aksed not to tune chips then we must
			// use the predefined value by default (to avoid Div-By-Zero error)
			// (ASIC_calculate_engines_nonce_range function uses this information)
			GLOBAL_CHIP_FREQUENCY_INFO[iHoveringChip] = (__ASIC_FREQUENCY_VALUES[__ASIC_FREQUENCY_ACTUAL_INDEX]);			
		#endif
		
		// Ok, Now we have the __chip_existance_map for this chip, the value can be used to set clock-enable register in this chip
		ASIC_set_clock_mask(iHoveringChip, __chip_existence_map[iHoveringChip]);
	}	
	
	// Ok, now we calculate nonce-range for the engines
	#if defined(__ACTIVATE_JOB_LOAD_BALANCING)
		ASIC_calculate_engines_nonce_range();
	#endif
	//__chip_existence_map[3] &= 0x0FF0;			//debug code
	// Do we run heavy diagnostics
	//while(1)
	//{
	#if defined(__RUN_HEAVY_DIAGNOSTICS_ON_EACH_ENGINE)
		ASIC_run_heavy_diagnostics();
		for(cChip=0; cChip<TOTAL_CHIPS_INSTALLED; cChip++)
		{
			for(cEngine=0; cEngine<16; cEngine++)
			{
				if((__chip_existence_map[cChip] >> cEngine) & 0x01)
				{
					cGoodEngineCount[cChip] ++;
				}
			}
		}
	#endif
	//}
	
	// Do we run scattered diagnostics?
	#if defined(__RUN_SCATTERED_DIAGNOSTICS)
		GLOBAL_SCATTERED_DIAG_TIME = MACRO_GetTickCountRet;
		ASIC_run_scattered_diagnostics();	
		GLOBAL_SCATTERED_DIAG_TIME = MACRO_GetTickCountRet - GLOBAL_SCATTERED_DIAG_TIME;
	#endif
	
	// Also, clear Timestamping for Engine supervision
	#if defined(__ENGINE_ENABLE_TIMESTAMPING)
		for (iHoveringChip = 0; iHoveringChip < TOTAL_CHIPS_INSTALLED; iHoveringChip++)
		{
			for (iHoveringEngine = 0; iHoveringEngine < TOTAL_CHIPS_INSTALLED; iHoveringEngine++)
			{
				GLOBAL_ENGINE_PROCESSING_STATUS[iHoveringChip][iHoveringEngine] = 0;
				GLOBAL_ENGINE_PROCESSING_START_TIMESTAMP[iHoveringChip][iHoveringEngine] = 0;
				GLOBAL_ENGINE_PROCESSING_FAILURE_SCORES[iHoveringChip][iHoveringEngine] = 0;
			}
		}
	#endif
	

	
	// Ok, now we calculate nonce-range for the engines
	#if defined(__ACTIVATE_JOB_LOAD_BALANCING)
		ASIC_calculate_engines_nonce_range();
	#endif
	
	// Also remember how many engines were detected on startup
	if (GLOBAL_TotalEnginesDetectedOnStartup == 0)
	{
		GLOBAL_TotalEnginesDetectedOnStartup = ASIC_get_processor_count();
	}
}

// Runs one 8nonce job on all engines, waits for them to finish and offlines those who don't find relative nonces
void ASIC_run_heavy_diagnostics(void)
{
	job_packet jpDiag;
	//unsigned int iExpectedNonces[8] ;
	u8 cChip;
	u16 wEngine;
	u32 iTimeToWait;
	u32 iTimeHolder;
	u32 iVTime;
	unsigned char bWasAnyEngineDecommissioned;
	__CHIP_WORKING_STATE iStatRet;
	u8	cTempString[8];
	u8	cAlarmFlag;
	
	// ---------
	// We issue this job to all engines, and then read back their results. Those who don't finish on time will also be decommissioned (26 seconds Max timeout)
	// ---------
			
	// Set fan speed to highest for the test
	//__AVR32_FAN_SetSpeed(FAN_CONTROL_BYTE_VERY_FAST);
			
	// DB0280CA 5817463E AA058769 C3B38F4E 2CB0FA08 F397200A D10172CB 913A8DC7 ,EB6C6880 51A1AE5F 1A016164

	//jpDiag.midstate[0]  = 0xDC; jpDiag.midstate[1]  = 0x02; jpDiag.midstate[2] =  0x80; jpDiag.midstate[3] = 0xCA;
	
	jpDiag.midstate[0]  = 0xDB; jpDiag.midstate[1]  = 0x02; jpDiag.midstate[2] =  0x80; jpDiag.midstate[3] = 0xCA;
	jpDiag.midstate[4]  = 0x58; jpDiag.midstate[5]  = 0x17; jpDiag.midstate[6] =  0x46; jpDiag.midstate[7] = 0x3E;
	jpDiag.midstate[8]  = 0xAA; jpDiag.midstate[9]  = 0x05; jpDiag.midstate[10] = 0x87; jpDiag.midstate[11] = 0x69;
	jpDiag.midstate[12] = 0xC3; jpDiag.midstate[13] = 0xB3; jpDiag.midstate[14] = 0x8F; jpDiag.midstate[15] = 0x4E;
	jpDiag.midstate[16] = 0x2C; jpDiag.midstate[17] = 0xB0; jpDiag.midstate[18] = 0xFA; jpDiag.midstate[19] = 0x08;
	jpDiag.midstate[20] = 0xF3; jpDiag.midstate[21] = 0x97; jpDiag.midstate[22] = 0x20; jpDiag.midstate[23] = 0x0A;
	jpDiag.midstate[24] = 0xD1; jpDiag.midstate[25] = 0x01; jpDiag.midstate[26] = 0x72; jpDiag.midstate[27] = 0xCB;
	jpDiag.midstate[28] = 0x91; jpDiag.midstate[29] = 0x3A; jpDiag.midstate[30] = 0x8D; jpDiag.midstate[31] = 0xC7;
			
	jpDiag.block_data[0] = 0xEB ; jpDiag.block_data[1] = 0x6C ;  jpDiag.block_data[2] = 0x68 ; jpDiag.block_data[3] = 0x80;
	jpDiag.block_data[4] = 0x51 ; jpDiag.block_data[5] = 0xA1 ;  jpDiag.block_data[6] = 0xAE ; jpDiag.block_data[7] = 0x5F;
	jpDiag.block_data[8] = 0x1A ; jpDiag.block_data[9] = 0x01 ;  jpDiag.block_data[10] = 0x61; jpDiag.block_data[11] = 0x64;
			
			
	
		
	// Send this job to all engines
	for (cChip = 0; cChip < TOTAL_CHIPS_INSTALLED; cChip++ )
	{
		
		if (!CHIP_EXISTS(cChip)) continue;
		//ComTransmitData(COM2, ",W",2);	
		memcpy((void*)ChipMiningStatus[cChip].cMidstate, (void*)jpDiag.midstate, SHA_MIDSTATE_SIZE);
		memcpy((void*)ChipMiningStatus[cChip].cBlockdata, (void*)jpDiag.block_data, SHA_BLOCKDATA_SIZE);
		ChipMiningStatus[cChip].ChipState = MINING;
		ChipMiningStatus[cChip].cEngineDoneCount = 0;
		ChipMiningStatus[cChip].cNonceCount = 0;
		for (wEngine = 0; wEngine < 16; wEngine++)
		{
			#if defined(DO_NOT_USE_ENGINE_ZERO)
			if (wEngine == 0) continue;
			#endif
				
			// Is engine ok so far?
			if (!IS_PROCESSOR_OK(cChip, wEngine)) continue;
				
			//submit the job to this engine
			ASIC_job_issue_to_specified_engine(cChip, wEngine, TRUE, TRUE, 0, 0xFFFFFFFF);
			
		}
	}
	//ComTransmitData(COM2, "M",1);	
	// What is the desired frequency? We'll wait for that much + 40%
	iTimeToWait = (4294000 / __ASIC_FREQUENCY_VALUES[__ASIC_FREQUENCY_ACTUAL_INDEX]);
	//iTimeToWait += (iTimeToWait * 8) / 10;		//more than 80% delay is not enough
	iTimeToWait <<= 1;				// multi by 2 may get result, modified by JJ
	//iTimeToWait >>= 1;
	
	MCU_MainLED_Set();
	ChipMiningStatus[0].cEngineCount = 1;
	GetBFCalculateTime(0);
	StartSoftTimerEx(0, ChipMiningStatus[0].dwCalcTimePerChip);
	iTimeHolder = MACRO_GetTickCountRet;
	while(1)
	{
		cAlarmFlag = ChipSoftTimer[0].cTimerAlarm;
		iVTime = MACRO_GetTickCountRet;
		if (((iVTime - iTimeHolder) % 800000) <= 400000)
		{
			MCU_MainLED_Set();
		}			
		else
		{
			MCU_MainLED_Reset();
		}
		
		if(cAlarmFlag == TRUE) break;
	}
	ClearSoftTimerEx(0);
	ChipMiningStatus[0].cEngineCount = 0;
	/*
	// We wait for this long
	 iTimeHolder = MACRO_GetTickCountRet;
	 dwTimeStart = iTimeHolder;
	while (TRUE)
	{
		// Reset the watchdog
		WATCHDOG_RESET;
			
		// Keep track of time
		iVTime = MACRO_GetTickCountRet + 2;
		if (((iVTime - iTimeHolder) >> 10) > iTimeToWait) break;
			
		// Do the usual tasks
		// Do some light show, just to let everyone we're working
		if (((iVTime - iTimeHolder) % 800000) <= 400000)
		{
			MCU_MainLED_Set();
		}			
		else
		{
			MCU_MainLED_Reset();
		}			
	}
	dwTimeEnd = iVTime;
	
	MCU_MainLED_Set();*/
		
	// Ok, now we read back results. All engines should have the following nonces
	// 0F83379F,3E145360,64DD5309,C88E0D8E,DAFD7BE9,F4AD7CFD,F6DA3CAB,F9B1BC01
		
	// If this happens, we need to recalculate nonce range
	bWasAnyEngineDecommissioned = FALSE;	
		
	// Send this job to all engines
	for (cChip = 0; cChip < TOTAL_CHIPS_INSTALLED; cChip++ )
	{
		if (!CHIP_EXISTS(cChip)) continue;
			
		for (wEngine = 0; wEngine < 16; wEngine++)
		{
			#if defined(DO_NOT_USE_ENGINE_ZERO)
			if (wEngine == 0) continue;
			#endif
				
			// Is engine ok so far?
			if (!IS_PROCESSOR_OK(cChip, wEngine)) continue;
				
			// Clear nonce result buffer, temperarily use chip nonce count buffer for every engine
			ChipMiningStatus[cChip].cNonceCount = 0;

			//get job result and status from this engine
			iStatRet = ASIC_get_job_status_from_engine(cChip, wEngine);

			cTempString[0] = ',';
			cTempString[1] = _AUX_LEFT_HEX (iStatRet);
			cTempString[2] = _AUX_RIGHT_HEX(iStatRet);
			//cTempString[3] = 0;
			ComTransmitData(COM2, cTempString,3);
			if ((iStatRet == DONE_NO_NONCE) || (iStatRet == IDLE) || (iStatRet == MINING))
			{
				// This engine is DEAD!
				bWasAnyEngineDecommissioned = TRUE;
				DECOMMISSION_PROCESSOR(cChip, wEngine);
			}
			else
			{
				// Meaning ASIC_JOB_NONCE_FOUND
				#if defined(__HEAVY_DIAGNOSTICS_STRICT_8_NONCES)
					if (ChipMiningStatus[cChip].cNonceCount != 8)
					{
						// This engine is DEAD!
						bWasAnyEngineDecommissioned = TRUE;
						DECOMMISSION_PROCESSOR(cChip, wEngine);
					}
					else
					{
						// Now check nonce results
						// 0F83379F,3E145360,64DD5309,C88E0D8E,DAFD7BE9,F4AD7CFD,F6DA3CAB,F9B1BC01
						
						if ((ChipMiningStatus[cChip].dwNonceList[7] != 0x0F83379F) ||
						(ChipMiningStatus[cChip].dwNonceList[6] != 0x3E145360) ||
						(ChipMiningStatus[cChip].dwNonceList[5] != 0x64DD5309) ||
						(ChipMiningStatus[cChip].dwNonceList[4] != 0xC88E0D8E) ||
						(ChipMiningStatus[cChip].dwNonceList[3] != 0xDAFD7BE9) ||
						(ChipMiningStatus[cChip].dwNonceList[2] != 0xF4AD7CFD) ||
						(ChipMiningStatus[cChip].dwNonceList[1] != 0xF6DA3CAB) ||
						(ChipMiningStatus[cChip].dwNonceList[0] != 0xF9B1BC01))
						{
							DECOMMISSION_PROCESSOR(cChip, wEngine);
							bWasAnyEngineDecommissioned = TRUE;
						}
						else
						{
							// We're clear! Let engine stay...
						}
					}
					
				#elif (__HEAVY_DIAGNOSTICS_MODERATE_7_NONCES)
					if (ChipMiningStatus[cChip].cNonceCount < 6)
					{
						// This engine is DEAD!
						bWasAnyEngineDecommissioned = TRUE;
						DECOMMISSION_PROCESSOR(cChip, wEngine);
					}
				#elif (__HEAVY_DIAGNOSTICS_MODERATE_3_NONCES)
					if (ChipMiningStatus[cChip].cNonceCount < 4)
					{
						// This engine is DEAD!
						bWasAnyEngineDecommissioned = TRUE;
						DECOMMISSION_PROCESSOR(cChip, wEngine);
					}
					else
					{
						ChipMiningStatus[cChip].cEngineCount++;
					}
				#endif
			}

			
			// Clear the engine [ NOTE : CORRECT !!!!!!!!!!!! ]

			if (wEngine == 0)
			{
				__ASIC_WriteEngine(cChip, wEngine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_READ_REGISTERS_DONE_BIT | __ActualRegister0Value));
				__ASIC_WriteEngine(cChip, wEngine, ASIC_SPI_WRITE_REGISTER, (__ActualRegister0Value));
			}
			else
			{
				__ASIC_WriteEngine(cChip, wEngine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_READ_REGISTERS_DONE_BIT));
				__ASIC_WriteEngine(cChip, wEngine, ASIC_SPI_WRITE_REGISTER, (0));				
			}

		}
		GetBFCalculateTime(cChip);		//calculate every chip nonce time, about 15S

		ChipMiningStatus[cChip].ChipState = IDLE;
	}


	
	// Was any engine decommissioned? If so, recalculate nonce ranges
	if (bWasAnyEngineDecommissioned == TRUE)
	{
		ASIC_calculate_engines_nonce_range();
	}
}


// Runs 45 jobs in a scattered mode, offlines all engines who don't find their respective nonce
/*
void ASIC_run_scattered_diagnostics()
{
			job_packet* jpX;
		unsigned int* jpXNonceCount;
		unsigned int* jpXNonces;
		unsigned char iTestingJob;
		unsigned int iActualTime;
		unsigned int TimeAllowedToBeBusy;
		unsigned char bWasAnyEngineDecommissioned;
		unsigned char iChipNav;
		unsigned char iEngineNav;
		unsigned int iNonceList[16];
		unsigned int iNonceCount = 0;
		unsigned int iStatus = 0;
		unsigned int iToBeFoundNonce;
		unsigned char bFound;
		unsigned int iFoundNonce;
		unsigned char iTargetChip = 0;
		u16 iTargetEngine = 0;
	// Initialize the predefined jobs
	
	for (iTestingJob = 0; iTestingJob < __TOTAL_SCATTERED_JOBS_TO_TEST; iTestingJob++)
	{
		// Reset watchdog
		WATCHDOG_RESET;
		
		// Proceed
		//job_packet* jpX;
		//unsigned int* jpXNonceCount;
		//unsigned int* jpXNonces;
		
		// Run time assignment of the variables
		if (iTestingJob == 0) { jpX = (job_packet*)&Jb1; jpXNonceCount = (unsigned int*)&Jb1NonceCount; jpXNonces = (unsigned int*)Jb1Nonces;	}
		if (iTestingJob == 1) { jpX = (job_packet*)&Jb2; jpXNonceCount = (unsigned int*)&Jb2NonceCount; jpXNonces = (unsigned int*)Jb2Nonces;	}
		if (iTestingJob == 2) { jpX = (job_packet*)&Jb3; jpXNonceCount = (unsigned int*)&Jb3NonceCount; jpXNonces = (unsigned int*)Jb3Nonces;	}
		if (iTestingJob == 3) { jpX = (job_packet*)&Jb4; jpXNonceCount = (unsigned int*)&Jb4NonceCount; jpXNonces = (unsigned int*)Jb4Nonces;	}
		if (iTestingJob == 4) { jpX = (job_packet*)&Jb5; jpXNonceCount = (unsigned int*)&Jb5NonceCount; jpXNonces = (unsigned int*)Jb5Nonces;	}
		if (iTestingJob == 5) { jpX = (job_packet*)&Jb6; jpXNonceCount = (unsigned int*)&Jb6NonceCount; jpXNonces = (unsigned int*)Jb6Nonces;	}
		if (iTestingJob == 6) { jpX = (job_packet*)&Jb7; jpXNonceCount = (unsigned int*)&Jb7NonceCount; jpXNonces = (unsigned int*)Jb7Nonces;	}
		if (iTestingJob == 7) { jpX = (job_packet*)&Jb8; jpXNonceCount = (unsigned int*)&Jb8NonceCount; jpXNonces = (unsigned int*)Jb8Nonces;	}
		if (iTestingJob == 8) { jpX = (job_packet*)&Jb9; jpXNonceCount = (unsigned int*)&Jb9NonceCount; jpXNonces = (unsigned int*)Jb9Nonces;	}
		if (iTestingJob == 9) { jpX = (job_packet*)&Jb10; jpXNonceCount = (unsigned int*)&Jb10NonceCount; jpXNonces = (unsigned int*)Jb10Nonces;	}
		if (iTestingJob == 10) { jpX = (job_packet*)&Jb11; jpXNonceCount = (unsigned int*)&Jb11NonceCount; jpXNonces = (unsigned int*)Jb11Nonces;	}
		if (iTestingJob == 11) { jpX = (job_packet*)&Jb12; jpXNonceCount = (unsigned int*)&Jb12NonceCount; jpXNonces = (unsigned int*)Jb12Nonces;	}
		if (iTestingJob == 12) { jpX = (job_packet*)&Jb13; jpXNonceCount = (unsigned int*)&Jb13NonceCount; jpXNonces = (unsigned int*)Jb13Nonces;	}
		if (iTestingJob == 13) { jpX = (job_packet*)&Jb14; jpXNonceCount = (unsigned int*)&Jb14NonceCount; jpXNonces = (unsigned int*)Jb14Nonces;	}
		if (iTestingJob == 14) { jpX = (job_packet*)&Jb15; jpXNonceCount = (unsigned int*)&Jb15NonceCount; jpXNonces = (unsigned int*)Jb15Nonces;	}
		if (iTestingJob == 15) { jpX = (job_packet*)&Jb16; jpXNonceCount = (unsigned int*)&Jb16NonceCount; jpXNonces = (unsigned int*)Jb16Nonces;	}
		if (iTestingJob == 16) { jpX = (job_packet*)&Jb17; jpXNonceCount = (unsigned int*)&Jb17NonceCount; jpXNonces = (unsigned int*)Jb17Nonces;	}
		if (iTestingJob == 17) { jpX = (job_packet*)&Jb18; jpXNonceCount = (unsigned int*)&Jb18NonceCount; jpXNonces = (unsigned int*)Jb18Nonces;	}
		if (iTestingJob == 18) { jpX = (job_packet*)&Jb19; jpXNonceCount = (unsigned int*)&Jb19NonceCount; jpXNonces = (unsigned int*)Jb19Nonces;	}
		if (iTestingJob == 19) { jpX = (job_packet*)&Jb20; jpXNonceCount = (unsigned int*)&Jb20NonceCount; jpXNonces = (unsigned int*)Jb20Nonces;	}
		if (iTestingJob == 20) { jpX = (job_packet*)&Jb21; jpXNonceCount = (unsigned int*)&Jb21NonceCount; jpXNonces = (unsigned int*)Jb21Nonces;	}
		if (iTestingJob == 21) { jpX = (job_packet*)&Jb22; jpXNonceCount = (unsigned int*)&Jb22NonceCount; jpXNonces = (unsigned int*)Jb22Nonces;	}
		if (iTestingJob == 22) { jpX = (job_packet*)&Jb23; jpXNonceCount = (unsigned int*)&Jb23NonceCount; jpXNonces = (unsigned int*)Jb23Nonces;	}
		if (iTestingJob == 23) { jpX = (job_packet*)&Jb24; jpXNonceCount = (unsigned int*)&Jb24NonceCount; jpXNonces = (unsigned int*)Jb24Nonces;	}
		if (iTestingJob == 24) { jpX = (job_packet*)&Jb25; jpXNonceCount = (unsigned int*)&Jb25NonceCount; jpXNonces = (unsigned int*)Jb25Nonces;	}
		if (iTestingJob == 25) { jpX = (job_packet*)&Jb26; jpXNonceCount = (unsigned int*)&Jb26NonceCount; jpXNonces = (unsigned int*)Jb26Nonces;	}
		if (iTestingJob == 26) { jpX = (job_packet*)&Jb27; jpXNonceCount = (unsigned int*)&Jb27NonceCount; jpXNonces = (unsigned int*)Jb27Nonces;	}
		if (iTestingJob == 27) { jpX = (job_packet*)&Jb28; jpXNonceCount = (unsigned int*)&Jb28NonceCount; jpXNonces = (unsigned int*)Jb28Nonces;	}
		if (iTestingJob == 28) { jpX = (job_packet*)&Jb29; jpXNonceCount = (unsigned int*)&Jb29NonceCount; jpXNonces = (unsigned int*)Jb29Nonces;	}
		if (iTestingJob == 29) { jpX = (job_packet*)&Jb30; jpXNonceCount = (unsigned int*)&Jb30NonceCount; jpXNonces = (unsigned int*)Jb30Nonces;	}
		if (iTestingJob == 30) { jpX = (job_packet*)&Jb31; jpXNonceCount = (unsigned int*)&Jb31NonceCount; jpXNonces = (unsigned int*)Jb31Nonces;	}
		if (iTestingJob == 31) { jpX = (job_packet*)&Jb32; jpXNonceCount = (unsigned int*)&Jb32NonceCount; jpXNonces = (unsigned int*)Jb32Nonces;	}
		if (iTestingJob == 32) { jpX = (job_packet*)&Jb33; jpXNonceCount = (unsigned int*)&Jb33NonceCount; jpXNonces = (unsigned int*)Jb33Nonces;	}
		if (iTestingJob == 33) { jpX = (job_packet*)&Jb34; jpXNonceCount = (unsigned int*)&Jb34NonceCount; jpXNonces = (unsigned int*)Jb34Nonces;	}
		if (iTestingJob == 34) { jpX = (job_packet*)&Jb35; jpXNonceCount = (unsigned int*)&Jb35NonceCount; jpXNonces = (unsigned int*)Jb35Nonces;	}
		if (iTestingJob == 35) { jpX = (job_packet*)&Jb36; jpXNonceCount = (unsigned int*)&Jb36NonceCount; jpXNonces = (unsigned int*)Jb36Nonces;	}
		if (iTestingJob == 36) { jpX = (job_packet*)&Jb37; jpXNonceCount = (unsigned int*)&Jb37NonceCount; jpXNonces = (unsigned int*)Jb37Nonces;	}
		if (iTestingJob == 37) { jpX = (job_packet*)&Jb38; jpXNonceCount = (unsigned int*)&Jb38NonceCount; jpXNonces = (unsigned int*)Jb38Nonces;	}
		if (iTestingJob == 38) { jpX = (job_packet*)&Jb39; jpXNonceCount = (unsigned int*)&Jb39NonceCount; jpXNonces = (unsigned int*)Jb39Nonces;	}
		if (iTestingJob == 39) { jpX = (job_packet*)&Jb40; jpXNonceCount = (unsigned int*)&Jb40NonceCount; jpXNonces = (unsigned int*)Jb40Nonces;	}
		if (iTestingJob == 40) { jpX = (job_packet*)&Jb41; jpXNonceCount = (unsigned int*)&Jb41NonceCount; jpXNonces = (unsigned int*)Jb41Nonces;	}
		if (iTestingJob == 41) { jpX = (job_packet*)&Jb42; jpXNonceCount = (unsigned int*)&Jb42NonceCount; jpXNonces = (unsigned int*)Jb42Nonces;	}
		if (iTestingJob == 42) { jpX = (job_packet*)&Jb43; jpXNonceCount = (unsigned int*)&Jb43NonceCount; jpXNonces = (unsigned int*)Jb43Nonces;	}
		if (iTestingJob == 43) { jpX = (job_packet*)&Jb44; jpXNonceCount = (unsigned int*)&Jb44NonceCount; jpXNonces = (unsigned int*)Jb44Nonces;	}
		if (iTestingJob == 44) { jpX = (job_packet*)&Jb45; jpXNonceCount = (unsigned int*)&Jb45NonceCount; jpXNonces = (unsigned int*)Jb45Nonces;	}		
		
		// Ok, we have the job, proceed with it...
		ASIC_job_issue(jpX, 0x0, 0xFFFFFFFF, FALSE, 0, FALSE);
		
		// Ok, wait until the thing is finished
		iActualTime = MACRO_GetTickCountRet;
		TimeAllowedToBeBusy = 0;
		
		#if defined(__PRODUCT_MODEL_JALAPENO__)
			TimeAllowedToBeBusy = 850000; // i.e. 5GH/s
		#elif defined(__PRODUCT_MODEL_LITTLE_SINGLE__)
			TimeAllowedToBeBusy = 150000; // i.e. 27.8GH/s
		#elif defined(__PRODUCT_MODEL_SINGLE__)
			TimeAllowedToBeBusy = 79000; // i.e. 55GH/s
		#elif defined(__PRODUCT_MODEL_MINIRIG__)
			TimeAllowedToBeBusy = 79000; // i.e. 55GH/s
		#endif
		
		// Since we have found nonces, which nonces weren't found?
		bWasAnyEngineDecommissioned = FALSE;
		
		// Start waiting. If we remained busy for more than allowed busy time
		while (ASIC_is_processing())
		{
			WATCHDOG_RESET;
			if (MACRO_GetTickCountRet - iActualTime > TimeAllowedToBeBusy)
			{
				// Then we have to deactivate engines...
				for (iChipNav = 0; iChipNav < TOTAL_CHIPS_INSTALLED; iChipNav++)
				{
					WATCHDOG_RESET;
					if (!CHIP_EXISTS(iChipNav)) continue;
					
					for (iEngineNav = 0; iEngineNav < 16; iEngineNav++)
					{
						#if defined(DO_NOT_USE_ENGINE_ZERO)
							if (iEngineNav == 0) continue;
						#endif
						
						if (!IS_PROCESSOR_OK(iChipNav, iEngineNav)) continue;
						
						// Ok, we discard the engine if it's busy
						if (ASIC_is_engine_processing(iChipNav, iEngineNav))
						{
							// Decommission it
							ASIC_reset_engine(iChipNav, iEngineNav);
							DECOMMISSION_PROCESSOR(iChipNav, iEngineNav);
							bWasAnyEngineDecommissioned = TRUE;
						}
					}
				}
				
				// Now that we're done, exit
				break;
			}
		}
		
		// Ok, now we check if all nonces were found
		//unsigned int iNonceList[16];
		iNonceCount = 0;
		iStatus = 0;
		
		iStatus = ASIC_get_job_status((unsigned int *)iNonceList, &iNonceCount, FALSE, 0);
		
		if ((iStatus == ASIC_JOB_NONCE_PROCESSING) ||
			(iStatus == ASIC_JOB_NONCE_NO_NONCE) ||
			(iStatus == ASIC_JOB_IDLE))
		{
			// Ok, we give up, just continue...
			continue;			
		}
		
		for (iToBeFoundNonce = 0; iToBeFoundNonce < *jpXNonceCount; iToBeFoundNonce++)
		{
			bFound = FALSE;
			
			for (iFoundNonce = 0; iFoundNonce < iNonceCount; iFoundNonce++)
			{
				if (jpXNonces[iToBeFoundNonce] == iNonceList[iFoundNonce])
				{
					bFound = TRUE;
					break;
				}
			}
			
			// Ok, if this nonce was not found, the break the whole deal
			if (bFound == FALSE)
			{
				iTargetChip = 0;
				iTargetEngine = 0;
				
				ASIC_find_nonce_designated_engine(jpXNonces[iToBeFoundNonce], &iTargetChip, &iTargetEngine);
				
				// Was it found?
				if (iTargetChip != 0xFF)
				{
					DECOMMISSION_PROCESSOR(iTargetChip, iTargetEngine);
					bWasAnyEngineDecommissioned = TRUE;
				}
			}
		}
		
		// Was any engine decommissioned? If so, we need to recalculate nonce range
		if (bWasAnyEngineDecommissioned == TRUE)
		{
			ASIC_calculate_engines_nonce_range();
		}	
	}
	
	
}
*/

// Finds which engine this nonce belongs to
void ASIC_find_nonce_designated_engine(unsigned int iNonce, unsigned char *iChip, u16 *iEngine)
{
	unsigned char iChipHover;
	u16 iEngineHover;
	*iChip = 0xFF;
	*iEngine = 0;
	
	// Scan all the engines in all the nonces
	for (iChipHover = 0; iChipHover < TOTAL_CHIPS_INSTALLED; iChipHover++)
	{
		if (!CHIP_EXISTS(iChipHover)) continue;
		
		for (iEngineHover = 0; iEngineHover < 16; iEngineHover++)
		{
			#if defined(DO_NOT_USE_ENGINE_ZERO)
				if (iEngineHover == 0) continue;
			#endif
			
			if (!IS_PROCESSOR_OK(iChipHover, iEngineHover)) continue;
			
			// Now start scanning
			if ((iNonce >= GLOBAL_CHIP_PROCESSOR_ENGINE_LOWBOUND[iChipHover][iEngineHover]) &&
			    (iNonce <= GLOBAL_CHIP_PROCESSOR_ENGINE_HIGHBOUND[iChipHover][iEngineHover]))
			{
				*iChip = iChipHover;
				*iEngine = iEngineHover;
				return;		
			}
		}
	}
}



// Sets the clock mask for a deisng
void ASIC_set_clock_mask(char iChip, unsigned int iClockMaskValue)
{
	__MCU_ASIC_Activate_CS();			//((iChip < 8) ? (1) : (2));
	#if defined(DISABLE_CLOCK_TO_ALL_ENGINES)
		__ASIC_WriteEngine(iChip, 0, ASIC_SPI_CLOCK_OUT_ENABLE, 0);	 // Disable all clocks
	#else
		__ASIC_WriteEngine(iChip, 0, ASIC_SPI_CLOCK_OUT_ENABLE, iClockMaskValue);
	#endif
	__MCU_ASIC_Deactivate_CS();			//((iChip < 8) ? (1) : (2));
}

// This function checks to see if the processor in this engine is correctly operating. We must find the correct nonce here...
char ASIC_diagnose_processor(u8 iChip, u16 iEngine)
{
	// By default it's set to true...
	unsigned char isProcessorOK = TRUE;
	unsigned char imi;
	__CHIP_WORKING_STATE ReadbackStatus;
	u32 ulBegin;
	u32 ulEnd;
	
	#if defined(DO_NOT_USE_ENGINE_ZERO)
		if (iEngine == 0) return FALSE;
	#endif
	
	// Our job-packet by default (Expected nonce is 8D9CB675 - Hence counter range is 8D9C670 to 8D9C67A)

	
	//for (index = 0; index < 12; index++) jp.block_data[index] = __stat_blockdata[index];
	//for (index = 0; index < 32; index++) jp.midstate[index]   = __stat_midstate[index];
	//jp.signature  = 0xAA;
	
	memcpy((void*)ChipMiningStatus[iChip].cMidstate, (void*)__stat_midstate, SHA_MIDSTATE_SIZE);
	memcpy((void*)ChipMiningStatus[iChip].cBlockdata, (void*)__stat_blockdata, SHA_BLOCKDATA_SIZE);
	
	
	// Run 20 Tests and check it every time...
	for (imi = 0; imi < __TOTAL_DIAGNOSTICS_RUN; imi++)
	{
		ChipMiningStatus[iChip].ChipState = MINING;
		ChipMiningStatus[iChip].cEngineDoneCount = 0;
		ChipMiningStatus[iChip].cNonceCount = 0;
		// Now send the job to the engine
		ASIC_job_issue_to_specified_engine(iChip,iEngine, TRUE, TRUE, 0x8D9CB670, 0x8D9CB67A);
	
		// Read back results immediately (with a little delay), it shouldv'e been finished since it takes about 40ns
		// Read back result. It should be DONE with FIFO zero of it being 0x8D9CB675
		ulBegin = MACRO_GetTickCountRet;
		ulEnd = 0;
		while (TRUE)
		{
			//WATCHDOG_RESET;
			
			// Read back the status
			ReadbackStatus = ASIC_get_job_status_from_engine(iChip, iEngine);
			ulEnd = MACRO_GetTickCountRet;
			if (ReadbackStatus == MINING)
			{
				if ((ulEnd - ulBegin) > 90)
				{
					// This means failure
					isProcessorOK = FALSE;
					break;
				}
				else
				{
					// Do nothing...
					// Just continue...
					isProcessorOK = TRUE;	
					continue;
				}
			}	
			else if(ReadbackStatus == DONE_HAVE_NONCE) 
			{
				// We're out of here..
				if((ChipMiningStatus[iChip].cNonceCount == 1) 
					|| (ChipMiningStatus[iChip].dwNonceList[0] == 0x8D9CB675))
				{
					isProcessorOK = TRUE;
					break;
				}
				else
				{
					isProcessorOK = FALSE;
					break;
				}
			}
			else if((ReadbackStatus == DONE_NO_NONCE) || (ReadbackStatus == IDLE))
			{
				isProcessorOK = FALSE;		//if idle, should be something wrong
				break;
			}
		}

		if(isProcessorOK == FALSE)
		{
			break;		//if engine fail, just return and report
		}
		else
		{
		
			// Set the second nonce if first nonce is correct
			// Now send the job to the engine
			ChipMiningStatus[iChip].ChipState = MINING;
			ChipMiningStatus[iChip].cNonceCount = 0;
			ASIC_job_issue_to_specified_engine(iChip,iEngine, TRUE, TRUE, 0x38E94200, 0x38E94300);
				
			// Read back results immediately (with a little delay), it shouldv'e been finished since it takes about 40ns
			// It should be DONE with FIFO zero of it being 0x38E9425A
			ulBegin = MACRO_GetTickCountRet;
			ulEnd = 0;
			while (TRUE)
			{
				// Read back the status
				ReadbackStatus = ASIC_get_job_status_from_engine(iChip, iEngine);
				ulEnd = MACRO_GetTickCountRet;
				if (ReadbackStatus == MINING)
				{
					if ((ulEnd - ulBegin) > 90)
					{
						// This means failure
						isProcessorOK = FALSE;
						break;
					}
					else
					{
						// Do nothing...
						// Just continue...
						isProcessorOK = TRUE;	
						continue;
					}
				}	
				else if(ReadbackStatus == DONE_HAVE_NONCE) 
				{
					// We're out of here..
					if((ChipMiningStatus[iChip].cNonceCount == 1) 
						|| (ChipMiningStatus[iChip].dwNonceList[0] == 0x38E9425A))
					{
						isProcessorOK = TRUE;
						break;
					}
					else
					{
						isProcessorOK = FALSE;
						break;
					}
				}
				else if((ReadbackStatus == DONE_NO_NONCE) || (ReadbackStatus == IDLE))
				{
					isProcessorOK = FALSE;		//if idle, should be something wrong
					break;
				}
			}
		}
		if(isProcessorOK == FALSE)
		{
			break;		//if engine fail, just return and report
		}
	}		
		
	return isProcessorOK;
}

/// This function tunes the CHIP to the requested frequency. If it's operating way below it,
/// then it will try to increase the frequency if needed. At the same time, it will see if the chip CAN
/// handle the frequency by checking the NONCE value. should it be wrong, then it will fall back
u32 ASIC_tune_chip_to_frequency(u8 cChip, u16 wEngine, char bOnlyReturnOperatingFrequency)
{
	// Our job-packet by default (Expected nonce is 8D9CB675 - Hence counter range is 8D9C670 to 8D9C67A)
	//static job_packet jp;
	//unsigned char index;
		// Actual hovering indexes
	char iActualHoverIndex = __ASIC_FREQUENCY_ACTUAL_INDEX;
	char bInFrequencyReductionState = FALSE;
	//char szTemp[256];
	u32 ulBegin;
	u32 ulEnd;
		// Read back result. It should be DONE with FIFO zero of it being 0x8D9CB675
	//unsigned int iReadbackNonce ;
	__CHIP_WORKING_STATE ReadbackStatus;
	unsigned char isProcessorOK = FALSE;
	float fDetectedFrequency;
	unsigned int iDetectedFrequency = 0;
	unsigned int iDiff;

	//char __stat_blockdata[] = {0xAC,0x84,0xF5,0x8B,0x4D,0x59,0xB7,0x4D,0x1B,0x2,0x85,0x52};
	//char __stat_midstate[]  = {0x3C,0x42,0x49,0xA8,0xE1,0xC5,0x45,0x78,0xA5,0x2D,0x83,0xC1,0x1,0xE5,0xC5,0x8E,0xF5,0x2F,0x3,0xD,0xEE,0x2E,0x9D,0x29,0xB6,0x94,0x9A,0xDF,0xA6,0x95,0x97,0xAE};
	//for (index = 0; index < 12; index++) jp.block_data[index] = __stat_blockdata[index];
	//for (index = 0; index < 32; index++) jp.midstate[index]   = __stat_midstate[index];
	//jp.signature  = 0xAA;
	
	memcpy((void*)ChipMiningStatus[cChip].cMidstate, (void*)__stat_midstate, SHA_MIDSTATE_SIZE);
	memcpy((void*)ChipMiningStatus[cChip].cBlockdata, (void*)__stat_blockdata, SHA_BLOCKDATA_SIZE);
	ChipMiningStatus[cChip].ChipState = MINING;
	ChipMiningStatus[cChip].cEngineDoneCount = 0;
	ChipMiningStatus[cChip].cNonceCount = 0;
		
	// Repeat this process until tuned
	while (TRUE)
	{
		// Now send the job to the engine
		ASIC_job_issue_to_specified_engine(cChip, wEngine, TRUE, TRUE, 0x8D98229A, 0x8D9CB67A); // 300,000 attempts will take 1.2ms per chip and gives us 1.7% precision
		//ASIC_job_start_processing(iChip, iEngineToUse, TRUE);
		
		// Hold the time
		ulBegin = MACRO_GetTickCountRet;
		ulEnd = 0;
	
		// Read back results immediately (with a little delay), it shouldv'e been finished since it takes about 40ns
		//NOP_OPERATION;
		//NOP_OPERATION;
		//NOP_OPERATION;
	
		// Read back result. It should be DONE with FIFO zero of it being 0x8D9CB675
		//iReadbackNonce = 0;
		//iReadbackStatus = 0;
		
		
		__MCU_ASIC_Activate_CS();			//((iChip < 8) ? (1) : (2));
	
		// Wait for the thing to finish...
		while (TRUE)
		{
			WATCHDOG_RESET;
			
			// Read back the status
			ReadbackStatus = ASIC_get_job_status_from_engine(cChip, wEngine);
			ulEnd = MACRO_GetTickCountRet;
			if (ReadbackStatus == MINING)
			{
				if ((ulEnd - ulBegin) > 9000)
				{
					// This means failure
					isProcessorOK = FALSE;
					break;
				}
				else
				{
					// Do nothing...
					// Just continue...
					isProcessorOK = TRUE;	
					continue;
				}
			}	
			else if(ReadbackStatus == DONE_HAVE_NONCE) 
			{
				// We're out of here..
				break;
			}
			else if((ReadbackStatus == DONE_NO_NONCE) || (ReadbackStatus == IDLE))
			{
				isProcessorOK = FALSE;		//if idle, should be something wrong
				break;
			}
		}

		// Is our processor bad? If so, just revert
		if (isProcessorOK == FALSE)
		{
			if (iActualHoverIndex == 0) // END OF LINE!!!
			{
				// We have a problem, abort...
				// Clear the fifo on engines
				ASIC_ReadComplete(cChip,wEngine);
				
				break;		//no available frequency
			}
			else
			{
				// This is being modified by functions execute after...
				// Hover back one index, set frequency and exit
				iActualHoverIndex -= 1;
				
				// Set the new frequency and exit... This will reduce the board operating speed however :(
				ASIC_SetFrequencyFactor(cChip, __ASIC_FREQUENCY_WORDS[iActualHoverIndex]);
				
				// Clear the fifo on engines
				ASIC_ReadComplete(cChip,wEngine);				
				
				// Unfortunately we have to try again, as the chip is not working properly...
				bInFrequencyReductionState = TRUE;
				continue;					
			}
		}

		//iReadbackNonce = __ASIC_ReadEngine(iChip, iEngineToUse, ASIC_SPI_FIFO0_LWORD) | (__ASIC_ReadEngine(iChip, iEngineToUse, ASIC_SPI_FIFO0_HWORD) << 16);
		else if (ChipMiningStatus[cChip].dwNonceList[0] != 0x8D9CB675) 
		{
			if (iActualHoverIndex == 0)
			{
				// We have a problem, abort...
				// Clear the fifo on engines
				ASIC_ReadComplete(cChip,wEngine);
				
				break;		//seek till end, no available frequency
			}
			else
			{
				// This is being modified by functions execute after...
				// Clear the fifo on engines
				ASIC_ReadComplete(cChip,wEngine);				
	
				// Hover back one index, set frequency and exit
				iActualHoverIndex -= 1;
	
				// Set the new frequency and exit... This will reduce the board operating speed however :(
				ASIC_SetFrequencyFactor(cChip, __ASIC_FREQUENCY_WORDS[iActualHoverIndex]);
				
				// Unfortunately we have to try again, as the chip is not working properly...
				bInFrequencyReductionState = TRUE;
				continue;
			}
		}
		else
		{
			// Finally deactivate the CS... we have processing to do...
			// Clear the fifo on engines
			ASIC_ReadComplete(cChip,wEngine);
			
			// Ok, all was ok to this point
			// Calculate frequency
			fDetectedFrequency = (1000000 / (ulEnd - ulBegin)) * 300000;
			iDetectedFrequency = (unsigned int)fDetectedFrequency;
			
			// Ok, is it slower more than 5MHz ? If so, do a jump
			iDiff = (iDetectedFrequency < __ASIC_FREQUENCY_WORDS[__ASIC_FREQUENCY_ACTUAL_INDEX]) ?  (__ASIC_FREQUENCY_WORDS[__ASIC_FREQUENCY_ACTUAL_INDEX] - iDetectedFrequency) : 0;
			
			// Now check, if we're in frequency reduction mode, it means that this is probably the highest frequency this chip can operate
			// without problem. If so, just return the actual value and get out, as there is no point in increasing frequency
			if (bInFrequencyReductionState == TRUE) break;  //found a frequency may get correct result
			
			// Are we just checking frequency? if so, return it here...
			if (bOnlyReturnOperatingFrequency == TRUE) break;  
								 
			// Is it slower by more than 5MHz?
			if (iDiff > 5)
			{
				// Check frequency, if it's at 8 then there is nothing to do
				if (iActualHoverIndex < __MAXIMUM_FREQUENCY_INDEX)
				{
					// Go faster...
					iActualHoverIndex += 1;
					
					// Set the new frequency and exit... This will reduce the board operating speed however :(
					ASIC_SetFrequencyFactor(cChip, __ASIC_FREQUENCY_WORDS[iActualHoverIndex]);
					
					// We are in regression test... Make sure the engine does operate correct with the new frequency
					continue;
				}
				else
				{
					// We've run out of numbers, just return the actual frequency
					break;
				}
			}		
			else // The number is within range, return it...
			{
				ChipMiningStatus[cChip].cFrequencySlect = iActualHoverIndex;
				// All ok, just exit...
				break;
			}
		}
	}		
	ChipMiningStatus[cChip].ChipState = IDLE;
	// Ok, we have to return the actual frequency
	//return 0; // Illegal value. This will cause the chip to be deactivated....
	return iDetectedFrequency;
}

// Say Read-Complete
void ASIC_ReadComplete(char iChip, u16 iEngine)
{
	__MCU_ASIC_Activate_CS();			//((iChip < 8) ? (1) : (2));
	if (iEngine == 0)
	{
		__ASIC_WriteEngine(iChip, iEngine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_READ_REGISTERS_DONE_BIT | __ActualRegister0Value));	
		__ASIC_WriteEngine(iChip, iEngine, ASIC_SPI_WRITE_REGISTER, (__ActualRegister0Value));	
	}
	else
	{
		__ASIC_WriteEngine(iChip, iEngine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_READ_REGISTERS_DONE_BIT));	
		__ASIC_WriteEngine(iChip, iEngine, ASIC_SPI_WRITE_REGISTER, (0));	
	}	
	__MCU_ASIC_Deactivate_CS();			//((iChip < 8) ? (1) : (2));
}	


// ---------------------------------------------------------------------------
//   MASK Bits
// ---------------------------------------------------------------------------
#define RESET_BIT		12
#define WRITE_VALID_BIT	11
#define READ_COMP_BIT	10
#define RESET_ERR_BIT	 9
#define BUSY_BIT		01
#define DONE_BIT		00

#define MASK_RESET_BIT		(0x1 << RESET_BIT)
#define MASK_WRITE_VALID_BIT	(0x1 << WRITE_VALID_BIT)
#define MASK_READ_COMP_BIT	(0x1 << READ_COMP_BIT)
#define MASK_RESET_ERR_BIT	(0x1 << RESET_ERR_BIT)

#define MASK_BUSY_BIT		(0x1 << BUSY_BIT)
#define MASK_DONE_BIT		(0x1 << DONE_BIT)

// ---------------------------------------------------------------------
//
//   MASKS FOR CLOCK SETUP and ENGINE RESET
//

#define MASK_CLK		(0x3000)
#define MASK_RESET_ENGINE_0	(0x1000)

extern int DATAREG0;
extern int DATAIN;
extern int DATAOUT;



//==================================================================
void __initEngines(u8 CHIP);
void __ResetSPIErrFlags(u8 CHIP, u16 ENGINE);
void __Reset_Engine(u8 CHIP, u16 ENGINE);

void __ResetSPIErrFlags(u8 CHIP, u16 ENGINE)
{
	if(ENGINE == 0){
		__ActualRegister0Value |= (MASK_RESET_ERR_BIT);
		DATAIN    = __ActualRegister0Value;
	} else {
		DATAIN = (MASK_RESET_ERR_BIT);
	}
	__ARM_SC_WriteData(CHIP,ENGINE,0x00, DATAIN);
	if(ENGINE == 0){
		__ActualRegister0Value &= ~(MASK_RESET_ERR_BIT);
		DATAIN = __ActualRegister0Value;
	} else {
		DATAIN &= ~(MASK_RESET_ERR_BIT);
	}
	__ARM_SC_WriteData(CHIP,ENGINE,0x00,DATAIN);
}

void __initEngines(u8 CHIP)
{
	//int i;
	int engineID;

	DATAREG0 = MASK_CLK;
	DATAIN = DATAREG0;
	__ARM_SC_WriteData(CHIP, 0, 0x00, DATAIN);	// --- Engine ID = 0

	// --- reseting all engines 1-15 ---
	DATAREG0 |= MASK_RESET_ENGINE_0;
	DATAIN = DATAREG0;
	__ARM_SC_WriteData(CHIP, 0, 0x61, DATAIN);	// --- Engine ID = 0

	DATAIN = 0x1000;			// --- Enable Clock Out for all Engines
	for(engineID = 1; engineID < 16; engineID++){
		__ARM_SC_WriteData(CHIP, engineID, 0x61, DATAIN);
	}

	DATAIN = 0x0000;			// --- Enable Clock Out for all Engines
	for(engineID = 1; engineID < 16; engineID++){
		__ARM_SC_WriteData(CHIP, engineID, 0x0, DATAIN);
	}

	DATAIN = 0x0000;			// --- Enable Clock Out for all Engines
	__ARM_SC_WriteData(CHIP, 0, 0x61, DATAIN);	// --- Engine ID = 0
}

void __Reset_Engine(u8 CHIP, u16 ENGINE)
{
	__ActualRegister0Value |= (MASK_RESET_ENGINE_0);
	DATAIN    = __ActualRegister0Value;

	__ARM_SC_WriteData(CHIP, ENGINE, 0x00, DATAIN);
	__ActualRegister0Value &= ~(MASK_RESET_ENGINE_0);
	DATAIN    = __ActualRegister0Value;

	// For engine zero, do not clear the reset
	__ARM_SC_WriteData(CHIP, ENGINE, 0x00, DATAIN);
}


void __Write_SPI(u8 iChip, u16 iEngine, u8 iAddress, u16 iData)
{
	//__MCU_ASIC_Activate_CS();		//((iChip < 8) ? (1) : (2));
	__ASIC_WriteEngine_NoCs(iChip, iEngine, iAddress, iData);
	//__MCU_ASIC_Deactivate_CS();		//((iChip < 8) ? (1) : (2));
}

u16 __Read_SPI(u8 iChip, u16 iEngine, u8 iAddress)
{
	u16 iRetVal = 0;
	//__MCU_ASIC_Activate_CS();			//((iChip < 8) ? (1) : (2));
	iRetVal = __ASIC_ReadEngine(iChip, iEngine, iAddress);
	//__MCU_ASIC_Deactivate_CS();		//((iChip < 8) ? (1) : (2));
	return iRetVal;
}

// We are going for 500MHz test


// Working...
void ASIC_Bootup_Chips()
{

	//unsigned int iHover = 0;
	u16 DATAIN;
	u8 CHIP = 0;
	//u16 wTemp;
	u32 dwDelay = 0x08FF;
		
	int c=0;
	
	int iDisableFlag = 0;
	
	// -----------------------------
	#if defined(DECOMISSION_CHIP_0)
		iDisableFlag |= (1<<0);
	#endif
	
	#if defined(DECOMISSION_CHIP_1)
		iDisableFlag |= (1<<1);
	#endif
	
	#if defined(DECOMISSION_CHIP_2)
		iDisableFlag |= (1<<2);
	#endif
	
	#if defined(DECOMISSION_CHIP_3)
		iDisableFlag |= (1<<3);
	#endif
	
	#if defined(DECOMISSION_CHIP_4)
		iDisableFlag |= (1<<4);
	#endif
	
	#if defined(DECOMISSION_CHIP_5)
		iDisableFlag |= (1<<5);
	#endif
	
	#if defined(DECOMISSION_CHIP_6)
		iDisableFlag |= (1<<6);
	#endif
	
	#if defined(DECOMISSION_CHIP_7)
		iDisableFlag |= (1<<7);
	#endif
	
	#if defined(DECOMISSION_CHIP_8)
		iDisableFlag |= (1<<8);
	#endif
	
	#if defined(DECOMISSION_CHIP_9)
		iDisableFlag |= (1<<9);
	#endif
	
	#if defined(DECOMISSION_CHIP_10)
		iDisableFlag |= (1<<10);
	#endif
	
	#if defined(DECOMISSION_CHIP_11)
		iDisableFlag |= (1<<11);
	#endif
	
	#if defined(DECOMISSION_CHIP_12)
		iDisableFlag |= (1<<12);
	#endif
	
	#if defined(DECOMISSION_CHIP_13)
		iDisableFlag |= (1<<13);
	#endif
	
	#if defined(DECOMISSION_CHIP_14)
		iDisableFlag |= (1<<14);
	#endif
	
	#if defined(DECOMISSION_CHIP_15)
		iDisableFlag |= (1<<15);
	#endif
	// --------------------------------
	
	// Operates at 250MHz, all Engines ACTIVE
	__MCU_ASIC_Activate_CS();
	#if !defined(FIVE_HUNDRED_MHZ_TEST)
		//for (CHIP = 0; CHIP < TOTAL_CHIPS_INSTALLED; CHIP++)
		for (CHIP = 3; CHIP < 4; CHIP++)
		{
			//Select ExtClk mode div1
			//DATAREG0[15]='0';//0=INT_CLK
			//DATAREG0[14]='0';//0=div2
			//DATAREG0[13]='1';//0=div4
			//DATAREG0[12]='0';//1=RESET
			//DATAREG0[0]='1'; //0=div 1
			DATAIN=0x2001;//INT_CLK, div/2
			__Write_SPI(CHIP,0,0x00,DATAIN);//int caddr, int engine, int reg, int data

			//wTemp = __ASIC_ReadEngine_NoCs(3, 0, 0);

			

			//Reset all instances (0-15):
			//Reset engine 0:
			//DATAREG0[12]='1'; 
			DATAIN=0x3001;//INT_CLK, div/2, reset
			__Write_SPI(CHIP,0,0x00,DATAIN);//int caddr, int engine, int reg, int data

			while((dwDelay--) != 0)
			{
			}		
			//Reset instances 1-15:
			DATAIN=0x1000;// bit[12]==reset
			for(c=1;c<16;c++)
			{
				__Write_SPI(CHIP,c,0,DATAIN);//int caddr, int engine, int reg, int data
				dwDelay = 0x02ff;
				while((dwDelay--) != 0)
			{
			}
			}
		
			//Enable Clock Out, all instances:
			DATAIN=0xFFFF;//
			__Write_SPI(CHIP,0,0x61, DATAIN);//int caddr, int engine, int reg, int data
			dwDelay = 0x02ff;
				while((dwDelay--) != 0)
				{
				}

			//Set Osc Control to slowest frequency:0000 (highest=0xCD55)
			/*
			#if defined(ASICS_OPERATE_AT_264MHZ)
				DATAIN = 0xD555; // Operates 250MHz
			#elif defined(ASICS_OPERATE_AT_250MHZ)
				DATAIN = 0xFF55; // Operates at 227MHz or less (TEMPORARLY)
			#elif defined(ASICS_OPERATE_AT_242MHZ)
				DATAIN = 0xFFF5; // Operates at 242MHz or less (TEMPORARLY)
			#elif defined(ASICS_OPERATE_AT_236MHZ)
				DATAIN = 0xFFFD; // Operates at 236MHz or less (TEMPORARLY)
			#elif defined(ASICS_OPERATE_AT_228MHZ)
				DATAIN = 0xFFFF; // Operates at 232MHz or less (TEMPORARLY)				
			#elif defined(ASICS_OPERATE_AT_261MHZ)
				DATAIN = 0xFD55; // Operates at 261MHz or less (TEMPORARLY)				
			#elif defined(ASICS_OPERATE_AT_280MHZ)
				DATAIN = 0x5555; // 280MHz
			#elif defined(ASICS_OPERATE_AT_170MHZ)
				DATAIN = 0x0000;
			#else
				DATAIN = 0x5555; // INVALID STATE
			#endif
			*/
						
			//DATAIN = 0xFFDF; // Operates 250MHz
			//DATAIN = 0x0000; // Operates 170MHz
			//DATAIN = 0xCDFF; // Operates 280MHz
			//DATAIN = 0xFFFF; // Operates Less than 250MHz			
			DATAIN = __ASIC_FREQUENCY_WORDS[__ASIC_FREQUENCY_ACTUAL_INDEX];
			__Write_SPI(CHIP,0,0x60,DATAIN);
			dwDelay = 0x02ff;
				while((dwDelay--) != 0)
			//Clear the Reset engine 1-15
			DATAIN=0x0000;
			for(c=1;c<16;c++){
				__Write_SPI(CHIP,c,0,DATAIN);
				dwDelay = 0x02ff;
				while((dwDelay--) != 0)
				{
				}
			}

			//Disable Clock Out, all engines
			/*
			#if defined(DISABLE_CLOCK_TO_ALL_ENGINES)
				DATAIN = 0; // Disable all clocks
			#else
				DATAIN = 0x0FFFE;
			#endif
			*/
			
			// is this chip disabled?			
			DATAIN= ((iDisableFlag & (1<<CHIP)) == 0) ? 0xFFFE : 0x0; // Engine 0 clock is not enabled here			
			__Write_SPI(CHIP,0,0x61,DATAIN);//int caddr, int engine, int reg, int data
			dwDelay = 0x02ff;
				while((dwDelay--) != 0)
				{
				}
		}	
	
	#else
	
		// Operates at 500MHz, engine 0 is active
		for (CHIP = 0; CHIP < TOTAL_CHIPS_INSTALLED; CHIP++)
		{
			//Select ExtClk mode div1
			//DATAREG0[15]='0';//0=INT_CLK
			//DATAREG0[14]='0';//0=div2
			//DATAREG0[13]='1';//0=div4
			//DATAREG0[12]='0';//1=RESET
			//DATAREG0[0]='1' ;//0=div 1
			DATAIN=(1 << 13) | (1<<14);//INT_CLK, (div/2 & div/4)
			__Write_SPI(CHIP,0,0x00,DATAIN);//int caddr, int engine, int reg, int data

			//Reset all instances (0-15):
			//Reset engine 0:
			//DATAREG0[12]='1';
			DATAIN=(1 << 13) | (1<<14) | (1<<12) ;//INT_CLK, (div/2 & div/4), reset
			__Write_SPI(CHIP,0,0x00,DATAIN);//int caddr, int engine, int reg, int data
		
			//Reset instances 1-15:
			DATAIN=0x1000;// bit[12]==reset
			for(c=1;c<16;c++)
			{
				__Write_SPI(CHIP,c,0,DATAIN);//int caddr, int engine, int reg, int data
			}
		
			//Enable Clock Out, all instances:
			DATAIN=0xFFFF; 
			__Write_SPI(CHIP,0,0x61, DATAIN);//int caddr, int engine, int reg, int data

			//Set Osc Control to slowest frequency:0000 (highest=0xCD55)
			/*
			#if defined(ASICS_OPERATE_AT_250MHZ)
				DATAIN = 0xFFD5; // Operates 250MHz
			#elif defined(ASICS_OPERATE_AT_280MHZ)
				DATAIN = 0x5555; // 280MHz
			#elif defined(ASICS_OPERATE_AT_170MHZ)
				DATAIN = 0x0000;
			#else
				DATAIN = 0x5555; // INVALID STATE
			#endif 
			*/
					
			DATAIN = __ASIC_FREQUENCY_WORDS[__ASIC_FREQUENCY_ACTUAL_INDEX];						
			__Write_SPI(CHIP,0,0x60,DATAIN);

			//Clear the Reset engine 1-15
			DATAIN=0x0000;
			for(c=1;c<16;c++){
				__Write_SPI(CHIP,c,0,DATAIN);
			}

			DATAIN = ((iDisableFlag & (1<<CHIP)) == 0) ? 0xFFFE : 0x0; // Engine 0 clock is not enabled here
			
			// DATAIN = 0x0FFFE;
			__Write_SPI(CHIP,0,0x61,DATAIN);
		}
	
	#endif
	__MCU_ASIC_Deactivate_CS();
	//CHIP STATE: Internal Clock, All Registers Reset,All BUT 0 Resets=0,
	//All clocks Disabled				
	return;				
}

int	ASIC_GetFrequencyFactor()
{
	// NOT IMPLEMENTED
	return 0;
}

void ASIC_SetFrequencyFactor(u8 cChip, u16 wFreqFactor)
{
	// if iChip is FF, then we set frequency for all
	__MCU_ASIC_Activate_CS();
	/*
	if (iChip == 0xFF)
	{
		__Write_SPI(iChip,0,0x60,iFreqFactor);	
		__Write_SPI(iChip,1,0x60,iFreqFactor);	
		__Write_SPI(iChip,2,0x60,iFreqFactor);	
		__Write_SPI(iChip,3,0x60,iFreqFactor);									
		__Write_SPI(iChip,4,0x60,iFreqFactor);
		__Write_SPI(iChip,5,0x60,iFreqFactor);
		__Write_SPI(iChip,6,0x60,iFreqFactor);
		__Write_SPI(iChip,7,0x60,iFreqFactor);
	}
	else
	{
		__Write_SPI(iChip,0,0x60,iFreqFactor); //int caddr, int engine, int reg, int data	
	}	*/

	//seems spi calling using wrong parameters, I'll try to change it below:
	if (cChip == 0xFF)
	{
		__ASIC_WriteEngine_NoCs(0,0,0x60,wFreqFactor);	
		__ASIC_WriteEngine_NoCs(1,0,0x60,wFreqFactor);	
		__ASIC_WriteEngine_NoCs(2,0,0x60,wFreqFactor);	
		__ASIC_WriteEngine_NoCs(3,0,0x60,wFreqFactor);									
		__ASIC_WriteEngine_NoCs(4,0,0x60,wFreqFactor);
		__ASIC_WriteEngine_NoCs(5,0,0x60,wFreqFactor);
		__ASIC_WriteEngine_NoCs(6,0,0x60,wFreqFactor);
		__ASIC_WriteEngine_NoCs(7,0,0x60,wFreqFactor);
	}
	else
	{
		__ASIC_WriteEngine_NoCs(cChip,0,0x60,wFreqFactor); 	
	}
	__MCU_ASIC_Deactivate_CS();	
}

void ASIC_WriteComplete(char iChip, u16 iEngine)
{
	// We're all set, for this chips... Tell the engine to start
	if (iEngine == 0)
	{
		__ASIC_WriteEngine(iChip, iEngine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_WRITE_REGISTERS_VALID_BIT | __ActualRegister0Value));
		__ASIC_WriteEngine(iChip, iEngine, ASIC_SPI_WRITE_REGISTER, (__ActualRegister0Value)); // Clear Write-Register Valid Bit
	}
	else
	{
		__ASIC_WriteEngine(iChip, iEngine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_WRITE_REGISTERS_VALID_BIT));
		__ASIC_WriteEngine(iChip, iEngine, ASIC_SPI_WRITE_REGISTER, 0); // Clear Write-Register Valid Bit
	}	
}

int ASIC_are_all_engines_done(unsigned int iChip)
{
	unsigned int imx;
	
	  unsigned char iExpectedEnginesToBeDone = 0;
	  unsigned char iTotalEnginesDone = 0;
	__MCU_ASIC_Activate_CS();			//((iChip < 8) ? (1) : (2));
	// Check all engines
	for (imx = 0; imx < 16; imx++)
	{
		// Is this processor ok? Because if it isn't, we shouldn't check it
		if (!IS_PROCESSOR_OK(iChip, imx)) continue;
		iExpectedEnginesToBeDone++;
		
		// Proceed...
		if ((__ARM_SC_ReadData(iChip, imx, ASIC_SPI_READ_STATUS_REGISTER) & ASIC_SPI_READ_STATUS_BUSY_BIT) == ASIC_SPI_READ_STATUS_BUSY_BIT) // Means it's busy
		{
			// __MCU_ASIC_Deactivate_CS();
			// return FALSE; // 0 = FALSE
		}
		else
		{
			// We set the flag here
			iTotalEnginesDone++;	
			
			// Also we update the flag for global supervision of engine activity, because THIS FUNCTION IS REPETITIVELY USED AND CALLED			
			#if defined(__ENGINE_ENABLE_TIMESTAMPING)
				GLOBAL_ENGINE_PROCESSING_STATUS[iChip][imx] = FALSE; // Meaning no longer processing			
			#endif
		}			

	}	
	
	__MCU_ASIC_Deactivate_CS();			//((iChip < 8) ? (1) : (2));	
		
	// We're ok
	return (iExpectedEnginesToBeDone == iTotalEnginesDone);
}

char ASIC_has_engine_finished_processing(char iChip, u16 iEngine)
{
	char bVal = ((__ARM_SC_ReadData(iChip, iEngine, ASIC_SPI_READ_STATUS_REGISTER) & ASIC_SPI_READ_STATUS_BUSY_BIT) != ASIC_SPI_READ_STATUS_BUSY_BIT);
	
	// Also we update the flag for global supervision of engine activity, because THIS FUNCTION IS REPETITIVELY USED AND CALLED
	#if defined(__ENGINE_ENABLE_TIMESTAMPING)
		if (bVal == TRUE)
		{
			GLOBAL_ENGINE_PROCESSING_STATUS[iChip][iEngine] = FALSE; // Meaning no longer processing	
		}		
	#endif	

	// return the value
	return bVal;
}

// How many total processors are there?
int ASIC_get_processor_count(void)
{
	  int iTotalProcessorCount = 0;
	  unsigned char xchip;
	  unsigned char yproc;
	//static int iTotalProcessorCountDetected = 0xFFFFFFFF;
	//if (iTotalProcessorCountDetected != 0xFFFFFFFF) return iTotalProcessorCountDetected;
	
	for (xchip =0; xchip < TOTAL_CHIPS_INSTALLED; xchip++)
	{
		if (!CHIP_EXISTS(xchip)) continue;
		
		for (yproc = 0; yproc < 16; yproc++)
		{
			#if defined(DO_NOT_USE_ENGINE_ZERO)
				if (yproc == 0) continue;
			#endif
			
			if (IS_PROCESSOR_OK(xchip, yproc)) iTotalProcessorCount++;	
		}
	}
	
	//iTotalProcessorCountDetected = iTotalProcessorCount;
	return iTotalProcessorCount;
}

// Calculate the lower-range and upper-range of each engine on the board
void ASIC_calculate_engines_nonce_range(void)
{	
	char vChip;
	unsigned int iRes;
	unsigned int iVFR;
	float fAttempt_Per_Unit_In_Total;
	float fChipShare ;
	unsigned int  iActualLowBound = 0;
	unsigned int  iChipInitialBound = 0;
	unsigned int  iChipShare = 0;
	unsigned int  iEngineShare = 0;
	unsigned char iLastEnginesIndex = 0;
	unsigned char iLastChipIndex = 0;
	char vEngine;

	// First find total processing power = Sigma(total engines per chip x speed of the chip)
	unsigned int iTotalProcessingPower = 0;
	for (vChip = 0; vChip < TOTAL_CHIPS_INSTALLED; vChip++)
	{
		if (!CHIP_EXISTS(vChip)) continue;
		iTotalProcessingPower += (GLOBAL_CHIP_FREQUENCY_INFO[vChip] * ASIC_get_chip_processor_count(vChip));
	}
	
	// Is total processing power ZERO? If so, this will cause a infinite reset loop.
	// Ask the user to powerup the board
	// WARNING: BOARD FAILED, ASICS REQUIRE POWER CYCLE
	if (iTotalProcessingPower == 0)
	{
		  iVFR = MACRO_GetTickCountRet;
		while (TRUE)
		{
			WATCHDOG_RESET;
			iRes = MACRO_GetTickCountRet + 1 - iVFR;
			if ((iRes % 200000) < 100000)
			{
				MCU_MainLED_Set();
			}
			else
			{
				MCU_MainLED_Reset();
			}
		}
	}
	
	// Now we have total processing power, see the TotalAttempts / TotalProcessingPower
	fAttempt_Per_Unit_In_Total = (0xFFFFFFFF) / iTotalProcessingPower;
	
	// Now Calculate SharePerChip (equaling fAttempt_Per_Unit_In_Total x (total-engines-in-chip x chip-freq)
	fChipShare = 0;
	
	// Our low and high bound values
	iActualLowBound = 0;
	iChipInitialBound = 0;
	iChipShare = 0;
	iEngineShare = 0;
	iLastEnginesIndex = 0;
	iLastChipIndex = 0;
		
	// Calculate per chip...
	for (vChip = 0; vChip < TOTAL_CHIPS_INSTALLED; vChip++)
	{
		// Does chip exist?
		if (!CHIP_EXISTS(vChip)) continue;
		
		// Calculate how much should be given to the chip
		fChipShare = fAttempt_Per_Unit_In_Total * (ASIC_get_chip_processor_count(vChip) * GLOBAL_CHIP_FREQUENCY_INFO[vChip]);
		
		// Now we know the chip share
		iChipShare = (unsigned int)fChipShare;
		iChipInitialBound = iActualLowBound;
		
		// What is the engine share here?
		iEngineShare = iChipShare / ASIC_get_chip_processor_count(vChip);
		
		// Anyway..., give engines their share...
		for (vEngine = 0; vEngine < 16; vEngine++)
		{
			// Allowed to use engine 0?
			#if defined(DO_NOT_USE_ENGINE_ZERO)
				if (vEngine == 0) continue;
			#endif
			
			// Reset the GLOBAL_ENGINE_MAXIMUM_OPERATING_TIME
			GLOBAL_ENGINE_MAXIMUM_OPERATING_TIME[vChip][vEngine] = 0;
			
			// Is the engine OK?
			if (!IS_PROCESSOR_OK(vChip, vEngine)) continue;
			
			// Give it the bound
			GLOBAL_CHIP_PROCESSOR_ENGINE_LOWBOUND[vChip][vEngine] =	iActualLowBound;
			GLOBAL_CHIP_PROCESSOR_ENGINE_HIGHBOUND[vChip][vEngine] = iActualLowBound + iEngineShare + ((iEngineShare > 1) ? (-1) : 0);
			iActualLowBound += iEngineShare;
			
			// Also set it's maximum operating time
			if (GLOBAL_CHIP_FREQUENCY_INFO[vChip] != 0)
			{
				#if defined(QUEUE_OPERATE_ONE_JOB_PER_CHIP)
					if (ASIC_get_chip_processor_count(vChip) > 0)
					{
						GLOBAL_ENGINE_MAXIMUM_OPERATING_TIME[vChip][vEngine] = (((0xFFFFFFFF / GLOBAL_CHIP_FREQUENCY_INFO[vChip] / (ASIC_get_chip_processor_count(vChip))) * 120) / 100); // The result is in microseconds	(+20% overhead)	
					}					
				#else // One Job Per Board
					GLOBAL_ENGINE_MAXIMUM_OPERATING_TIME[vChip][vEngine] = (iEngineShare / GLOBAL_CHIP_FREQUENCY_INFO[vChip]); // The result is in microseconds	
				#endif
				
			}			
			
			// Remember the last engines index
			iLastEnginesIndex = vEngine;
		}
		
		// Set last engines upper bound to the correct value
		GLOBAL_CHIP_PROCESSOR_ENGINE_HIGHBOUND[vChip][iLastEnginesIndex] = iChipInitialBound + iChipShare + ((iChipShare > 1) ? (-1) : 0);
		if (GLOBAL_CHIP_PROCESSOR_ENGINE_HIGHBOUND[vChip][iLastEnginesIndex] != 0) GLOBAL_CHIP_PROCESSOR_ENGINE_HIGHBOUND[vChip][iLastEnginesIndex]--; 
		
		// Now, set the actual low bound to chip initial low bound + 
		iActualLowBound = iChipInitialBound + iChipShare;
		
		// Remember the last chip we visited
		iLastChipIndex = vChip;
	}
	
	// Correct the last chips last engines upper board
	GLOBAL_CHIP_PROCESSOR_ENGINE_HIGHBOUND[iLastChipIndex][iLastEnginesIndex] = 0xFFFFFFFF;
		
	// We're done...
}

// How many total processors are there?
int ASIC_get_chip_processor_count(char iChip)
{
	int iTotalProcessorCount = 0;
	unsigned char yproc;
	if (!CHIP_EXISTS(iChip)) return 0;	
	for (yproc = 0; yproc < 16; yproc++) { if (IS_PROCESSOR_OK(iChip, yproc)) iTotalProcessorCount++;	}
	return iTotalProcessorCount;
}

//This function only called when chip is mining!!!!, the caller should be know if this engine is idle!!!
__CHIP_WORKING_STATE ASIC_get_job_status(u8 cChip)
{
	int y_engine;
	u32 dwNonce;
	u8	cTempString[10];
	u16 i_status_reg;
	u8	cIdleEngineCount = 0;
	__CHIP_WORKING_STATE TempChipStatus;

	if(ChipMiningStatus[cChip].cEngineDoneCount == cGoodEngineCount[cChip])
	{
		if(ChipMiningStatus[cChip].cNonceCount != 0)
		{
			TempChipStatus = DONE_HAVE_NONCE;
		}
		else
		{
			TempChipStatus = DONE_NO_NONCE;
		}
		return TempChipStatus;		//enter this branch, means job result FIFO is full
	}
	
	// Test all engines
	ComTransmitData(COM2, "r",1);
	//UartBackDoorByte(cChip);
	for (y_engine = 0; y_engine < 16; y_engine++)
	{
		// skip bad engine
		if (!IS_PROCESSOR_OK(cChip,  y_engine)) continue;
		
		// read out engine status register
		i_status_reg = __ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_READ_STATUS_REGISTER);

		cTempString[0] = ',';
		cTempString[1] = __aux_CharMap[(i_status_reg >> 12)& 0x0F];
		cTempString[2] = __aux_CharMap[(i_status_reg >>8)& 0x0F];
		cTempString[3] = __aux_CharMap[(i_status_reg >>4)& 0x0F];
		cTempString[4] = __aux_CharMap[(i_status_reg)& 0x0F];
		ComTransmitData(COM2, cTempString,5);

		//if engine done, process result
		if ((i_status_reg & ASIC_SPI_READ_STATUS_DONE_BIT) == ASIC_SPI_READ_STATUS_DONE_BIT)
		{	
			// Check nonce existence
			if (((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH1_BIT) != 0) 
					&& (ChipMiningStatus[cChip].cNonceCount < MAX_NONCE_IN_RESULT))
			{ 
				dwNonce = (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO0_LWORD)) | 
						  (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO0_HWORD) << 16); 
				ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
				ChipMiningStatus[cChip].cNonceCount ++;
				UartBackDoor(dwNonce);
			}
			if (((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH2_BIT) != 0)
					&& (ChipMiningStatus[cChip].cNonceCount < MAX_NONCE_IN_RESULT))
			{ 
				dwNonce = (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO1_LWORD)) | 
						  (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO1_HWORD) << 16); 
				ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
				ChipMiningStatus[cChip].cNonceCount ++;
				UartBackDoor(dwNonce);
			}
			if (((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH3_BIT) != 0)
					&& (ChipMiningStatus[cChip].cNonceCount < MAX_NONCE_IN_RESULT))
			{ 
				dwNonce = (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO2_LWORD)) | 
						  (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO2_HWORD) << 16); 
				ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
				ChipMiningStatus[cChip].cNonceCount ++;
				UartBackDoor(dwNonce);
			}
			if (((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH4_BIT) != 0)
					&& (ChipMiningStatus[cChip].cNonceCount < MAX_NONCE_IN_RESULT))
			{ 
				dwNonce = (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO3_LWORD)) | 
						  (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO3_HWORD) << 16); 
				ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
				ChipMiningStatus[cChip].cNonceCount ++;
				UartBackDoor(dwNonce);
			}
			if (((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH5_BIT) != 0)
					&& (ChipMiningStatus[cChip].cNonceCount < MAX_NONCE_IN_RESULT))
			{ 
				dwNonce = (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO4_LWORD)) | 
						  (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO4_HWORD) << 16); 
				ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
				ChipMiningStatus[cChip].cNonceCount ++;
				UartBackDoor(dwNonce);
			}
			if (((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH6_BIT) != 0)
					&& (ChipMiningStatus[cChip].cNonceCount < MAX_NONCE_IN_RESULT))
			{ 
				dwNonce = (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO5_LWORD)) | 
						  (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO5_HWORD) << 16); 
				ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
				ChipMiningStatus[cChip].cNonceCount ++;
				UartBackDoor(dwNonce);
			}
			if (((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH7_BIT) != 0)
					&& (ChipMiningStatus[cChip].cNonceCount < MAX_NONCE_IN_RESULT))
			{ 
				dwNonce = (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO6_LWORD)) | 
						  (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO6_HWORD) << 16); 
				ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
				ChipMiningStatus[cChip].cNonceCount ++;
				UartBackDoor(dwNonce);
			}
			if (((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH8_BIT) != 0)
					&& (ChipMiningStatus[cChip].cNonceCount < MAX_NONCE_IN_RESULT))
			{ 
				dwNonce = (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO7_LWORD)) | 
						  (__ASIC_ReadEngine(cChip, y_engine, ASIC_SPI_FIFO7_HWORD) << 16); 
				ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
				ChipMiningStatus[cChip].cNonceCount ++;
				UartBackDoor(dwNonce);
			}

			// Clear the engine FIFO when finished read out nonce[ NOTE : CORRECT !!!!!!!!!!!! ]
			// Even the nonce result count is full, we need to clear the rest engine FIFO
			if (y_engine == 0)
			{
				__ASIC_WriteEngine(cChip, y_engine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_READ_REGISTERS_DONE_BIT | __ActualRegister0Value));
				__ASIC_WriteEngine(cChip, y_engine, ASIC_SPI_WRITE_REGISTER, (__ActualRegister0Value));
			}
			else
			{
				__ASIC_WriteEngine(cChip, y_engine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_READ_REGISTERS_DONE_BIT));
				__ASIC_WriteEngine(cChip, y_engine, ASIC_SPI_WRITE_REGISTER, (0));				
			}

			ChipMiningStatus[cChip].cEngineDoneCount ++;
		}
			
		// Is this engine BUSY?
		else if ((i_status_reg &  ASIC_SPI_READ_STATUS_BUSY_BIT) == ASIC_SPI_READ_STATUS_BUSY_BIT)
		{
			continue; 
		}

		// Is this engine IDLE?
		else
		{
			cIdleEngineCount ++;
			continue;
		}
	}
	if(ChipMiningStatus[cChip].cEngineDoneCount == cGoodEngineCount[cChip]) 
	{
		if(ChipMiningStatus[cChip].cNonceCount != 0)
		{
			TempChipStatus = DONE_HAVE_NONCE;
		}
		else
		{
			TempChipStatus = DONE_NO_NONCE;
		}
	}
	else if(cIdleEngineCount == cGoodEngineCount[cChip])		//a protection brach, should not enter here when function called in idle state
	{
		TempChipStatus = IDLE;
	}
	else 
	{
		TempChipStatus = MINING;
	}

	return TempChipStatus;
}

__CHIP_WORKING_STATE ASIC_get_job_status_from_engine(u8 cChip, u16 wEngine)
{
	unsigned int i_status_reg;
	u32 dwNonce;
	__CHIP_WORKING_STATE TempChipStatus;

	// Proceed
	i_status_reg = __ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_READ_STATUS_REGISTER);
	//ComTransmitData(COM2, ",",1);
	//UartBackDoorWord(wEngine);
	//UartBackDoorWord(i_status_reg);

	//if engine done, process result
	if ((i_status_reg & ASIC_SPI_READ_STATUS_DONE_BIT) == ASIC_SPI_READ_STATUS_DONE_BIT)
	{	
		// Check nonce existence
		if ((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH1_BIT) != 0)
		{ 
			dwNonce = (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO0_LWORD)) | 
					  (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO0_HWORD) << 16); 
			ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
			ChipMiningStatus[cChip].cNonceCount ++;
			UartBackDoor(dwNonce);
		}
		if ((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH2_BIT) != 0)
		{ 
			dwNonce = (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO1_LWORD)) | 
					  (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO1_HWORD) << 16); 
			ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
			ChipMiningStatus[cChip].cNonceCount ++;
			UartBackDoor(dwNonce);
		}
		if ((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH3_BIT) != 0)
		{ 
			dwNonce = (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO2_LWORD)) | 
					  (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO2_HWORD) << 16); 
			ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
			ChipMiningStatus[cChip].cNonceCount ++;
			UartBackDoor(dwNonce);
		}
		if ((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH4_BIT) != 0)
		{ 
			dwNonce = (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO3_LWORD)) | 
					  (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO3_HWORD) << 16); 
			ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
			ChipMiningStatus[cChip].cNonceCount ++;
			UartBackDoor(dwNonce);
		}
		if ((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH5_BIT) != 0)
		{ 
			dwNonce = (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO4_LWORD)) | 
					  (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO4_HWORD) << 16); 
			ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
			ChipMiningStatus[cChip].cNonceCount ++;
			UartBackDoor(dwNonce);
		}
		if ((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH6_BIT) != 0)
		{ 
			dwNonce = (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO5_LWORD)) | 
					  (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO5_HWORD) << 16); 
			ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
			ChipMiningStatus[cChip].cNonceCount ++;
			UartBackDoor(dwNonce);
		}
		if ((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH7_BIT) != 0)
		{ 
			dwNonce = (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO6_LWORD)) | 
					  (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO6_HWORD) << 16); 
			ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
			ChipMiningStatus[cChip].cNonceCount ++;
			UartBackDoor(dwNonce);
		}
		if ((i_status_reg & ASIC_SPI_READ_STATUS_FIFO_DEPTH8_BIT) != 0)
		{ 
			dwNonce = (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO7_LWORD)) | 
					  (__ASIC_ReadEngine(cChip, wEngine, ASIC_SPI_FIFO7_HWORD) << 16); 
			ChipMiningStatus[cChip].dwNonceList[ChipMiningStatus[cChip].cNonceCount]= dwNonce; 
			ChipMiningStatus[cChip].cNonceCount ++;
			UartBackDoor(dwNonce);
		}

		// Clear the engine FIFO when finished read out nonce[ NOTE : CORRECT !!!!!!!!!!!! ]
		// Even the nonce result count is full, we need to clear the rest engine FIFO
		if (wEngine == 0)
		{
			__ASIC_WriteEngine(cChip, wEngine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_READ_REGISTERS_DONE_BIT | __ActualRegister0Value));
			__ASIC_WriteEngine(cChip, wEngine, ASIC_SPI_WRITE_REGISTER, (__ActualRegister0Value));
		}
		else
		{
			__ASIC_WriteEngine(cChip, wEngine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_READ_REGISTERS_DONE_BIT));
			__ASIC_WriteEngine(cChip, wEngine, ASIC_SPI_WRITE_REGISTER, (0));				
		}

		if(ChipMiningStatus[cChip].cNonceCount != 0)
		{
			TempChipStatus = DONE_HAVE_NONCE;
		}
		else
		{
			TempChipStatus = DONE_NO_NONCE;
		}
		
	}
	else if((i_status_reg &  ASIC_SPI_READ_STATUS_BUSY_BIT) == ASIC_SPI_READ_STATUS_BUSY_BIT)
	{
		TempChipStatus = MINING;
	}
	else
	{
		TempChipStatus = IDLE;
	}
			
	return TempChipStatus;
}


//#define MACRO__ASIC_WriteEngine(x,y,z,d)				__AVR32_SC_WriteData(x,y,z,d);
#define MACRO__ASIC_WriteEngine(x,y,z,d) __ASIC_WriteEngine(x,y,z,d);
//#define MACRO__ASIC_WriteEngine_SecondBank(x,y,z,d)		__AVR32_SC_WriteData(x,y,z,d);


void MACRO__AVR32_SPI0_SendWord_Express2(u16 x)
{
		AVR32_SPI0_TDR = (x & 0x0FFFF); 
		while ((AVR32_SPI0_SR & (1 << 1)) == 0); 
}
	
void MACRO__AVR32_SPI1_SendWord_Express2(u16 x) \
{ 
	AVR32_SPI1_TDR = (x & 0x0FFFF); 
	while ((AVR32_SPI1_SR & (1 << 1)) == 0); 
}	

void  MACRO__ASIC_WriteEngineExpress(u8 x, u16 y, u8 z, u16 d) 
{ 
	//u16 iCommand = 0; 
	//iCommand = ((((u16)x) << 12 ) &  0x7000    ) | 
	//		   ((((u16)y) << 8  ) &  0x0F00    ) | 
	//		   ((((u16)z)       ) &  0x00FF    ); 	
	//MACRO__AVR32_SPI0_SendWord_Express2(iCommand); 
	//MACRO__AVR32_SPI0_SendWord_Express2(d); 
	__ASIC_WriteEngine_NoCs(x,y,z,d);
}
	
void MACRO__ASIC_WriteEngineExpress_SecondBank(u16 x, u16 y, u16 z, u16 d) \
{ 
	/*
	unsigned int iCommand = 0; 
	iCommand = (((unsigned int)(x   & 0x0FF) << 12 ) &  0x7000    ) | 
				(((unsigned int)(y   & 0x0FF) << 8  ) &  0x0F00    ) | 
				(((unsigned int)(z   & 0x0FF)       ) &  0x00FF    ); 
	MACRO__AVR32_SPI1_SendWord_Express2(iCommand); \
	MACRO__AVR32_SPI1_SendWord_Express2(d & 0x0FFFF); \
	*/
}	

/*void ASIC_job_issue(void* pJobPacket, 
					unsigned int _LowRange, 
					unsigned int _HighRange,
					const char bIssueToSingleChip,
					const char iChipToIssue,
					const char bAdd2msLatency)*/

//because only works at one job in one chip mode, only indicate chip address is enough
//before call this function, caller should fill job in ChipMiningStatus[iChip]
void ASIC_job_issue(u8 cChip)					
{
	//int iChipCount;
	//int iProcessorCount;
	unsigned int iRangeSlice;
	//unsigned int iRemainder;
	unsigned int iLowerRange;
	unsigned int iUpperRange;
	//int iTotalChipsHovered;
	//pjob_packet pjp;
	//unsigned char bIsThisOurFirstTime;
	u16 y_engine;
	//u8 cTempString[8];

	//Because the high level caller should check if this chip exist, by default this chip is good and idle when calling
	
	// Ok, since we're issuing a job, Activate pulsing if it has been requested
	#if defined(ENABLE_JOB_PULSING_SYSTEM)
		System_Request_Pulse_Blink();
	#endif
	
	iRangeSlice = 0xFFFFFFFF / cGoodEngineCount[cChip];
	  
	if(0xFFFFFFFF > (iRangeSlice * cGoodEngineCount[cChip]))
	{
		iRangeSlice ++;
	}
	iLowerRange = 0;
	iUpperRange = iRangeSlice;
	


	//ComTransmitData(COM2, "S",1);
	//UartBackDoorByte(cChip);
	// We assign the splitted job to each engine in this chip
	for (y_engine = 0; y_engine < 16; y_engine++)
	{
		// Is this processor healthy?
		if (!IS_PROCESSOR_OK(cChip, y_engine)) continue;
		ASIC_job_issue_to_specified_engine(cChip,y_engine,TRUE,TRUE,iLowerRange,iUpperRange);
		//ComTransmitData(COM2, "m",1);
	
		// Increase our range
		iLowerRange += iRangeSlice;
		iUpperRange += iRangeSlice;
		if (iUpperRange < iLowerRange) iUpperRange = 0xFFFFFFFF; // Last Number
		if (iUpperRange > 0x0FFFFFF00) iUpperRange = 0xFFFFFFFF;
		
	}	

	//after issue divided job to all engines in a chip, start the correspond chip soft timer
	//when soft timer alarm, then we can read out nonce result
	StartSoftTimerEx(cChip, ChipMiningStatus[cChip].dwCalcTimePerChip);
	
	// Chip Started, so make it blink
	if (GLOBAL_ChipActivityLEDCounter[cChip] == 0) { GLOBAL_ChipActivityLEDCounter[cChip] = 10; }
		
	
	// Ok It's no longer our fist time
	// WARNING: TO BE CORRECTED. THIS CAN CAUSE PROBLEM IF WE'RE ISSUEING TO A SINGLE ENGINE AND OTHERS ARENT INITIALIZED
	// THIS WILL NOT BE THE CASE TODAY AS WE DIAGNOSE THE ENGINES IN STARTUP. BUT IT COULD BE PROBLEMATIC IF WE DON'T DIAGNOSE
	// ON STARTUP (I.E. CHIPS WILL NOT HAVE THEIR STATIC VALUES SET)
	// ORIGINAL> if (bIssueToSingleChip == FALSE) bIsThisOurFirstTime = FALSE; // We only set it to false if we're sending the job to all engines on the BOARD
	//bIsThisOurFirstTime = FALSE; // We only set it to false if we're sending the job to all engines on the BOARD
	//ComTransmitData(COM2, "K",1);
	//if (bIssueToSingleChip == FALSE)
	//{
		// Set our timestamp
		GLOBAL_LastJobIssueToAllEngines = MACRO_GetTickCountRet;
	//}

	// Are we on OneJobPerChip?
	#if defined(QUEUE_OPERATE_ONE_JOB_PER_CHIP)
		GLOBAL_LastJobIssuedToChip[cChip] = MACRO_GetTickCountRet;	
	#endif

}

void ASIC_job_start_processing(char iChip, u16 iEngine, char bForcedStart)
{
	// Error Check: _HighRange - _LowRange must be at least 256
	char x_chip = iChip;
	u16 y_engine = iEngine;

	if (bForcedStart == FALSE)
	{
		if (!CHIP_EXISTS(iChip)) return;
		if (!IS_PROCESSOR_OK(x_chip, y_engine)) return;
	}
	
	// Activate the SPI
	__MCU_ASIC_Activate_CS();			//((iChip < 8) ? (1) : (2));
	//NOP_OPERATION;
	//NOP_OPERATION;	
	
	// Set Write-Register valid
	// We're all set, for this chips... Tell the engine to start
	if ((y_engine == 0))
	{
		//MACRO__ASIC_WriteEngine(x_chip, y_engine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_WRITE_REGISTERS_VALID_BIT | __ActualRegister0Value));
		//MACRO__ASIC_WriteEngine(x_chip, y_engine, ASIC_SPI_WRITE_REGISTER, (__ActualRegister0Value)); // Clear Write-Register Valid Bit

		__ASIC_WriteEngine(x_chip, y_engine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_WRITE_REGISTERS_VALID_BIT | __ActualRegister0Value));
		__ASIC_WriteEngine(x_chip, y_engine, ASIC_SPI_WRITE_REGISTER, (__ActualRegister0Value)); // Clear Write-Register Valid Bit
	}
	else
	{
		//MACRO__ASIC_WriteEngine(x_chip, y_engine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_WRITE_REGISTERS_VALID_BIT ));
		//MACRO__ASIC_WriteEngine(x_chip, y_engine, ASIC_SPI_WRITE_REGISTER, 0); // Clear Write-Register Valid Bit

		__ASIC_WriteEngine(x_chip, y_engine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_WRITE_REGISTERS_VALID_BIT ));
		__ASIC_WriteEngine(x_chip, y_engine, ASIC_SPI_WRITE_REGISTER, 0); // Clear Write-Register Valid Bit
	}
				
	// Deactivate the SPI
	__MCU_ASIC_Deactivate_CS();			//((iChip < 8) ? (1) : (2));
}


void ASIC_job_issue_to_specified_engine(u8  iChip, 
										u16  iEngine,
										char  bLoadStaticData,
										u16  bIgniteEngine,
										u32 _LowRange,
										u32 _HighRange)
{
	//int iTotalChipsHovered;
	//pjob_packet pjp;
	// Error Check: _HighRange - _LowRange must be at least 256
	//char x_chip = iChip;
	//char y_engine = iEngine;
	//ComTransmitData(COM2, "N",1);	
	//if (!CHIP_EXISTS(iChip)) return;
	//if (!IS_PROCESSOR_OK(x_chip, y_engine)) return;
	
	// STATIC RULE - Engine 0 not used
	#if defined(DO_NOT_USE_ENGINE_ZERO)
		if (iEngine == 0) return; // Do not start engine 0
	#endif
	
	  //iTotalChipsHovered = 0;
	//pjp = (pjob_packet)(pJobPacket);
	
	// Do we need to export spreads?
	#if defined(__EXPORT_ENGINE_RANGE_SPREADS)
		__ENGINE_LOWRANGE_SPREADS[iChip][iEngine] = _LowRange;
		__ENGINE_HIGHRANGE_SPREADS[iChip][iEngine] = _HighRange;
	#endif
	
	// Activate the SPI
	__MCU_ASIC_Activate_CS();			//((iChip < 8) ? (1) : (2));
	//NOP_OPERATION;
	//NOP_OPERATION;
	
	// Reset the engine
	__ASIC_WriteEngine_NoCs(iChip,iEngine, 0, (1<<9) | (1<<12));
	__ASIC_WriteEngine_NoCs(iChip,iEngine, 0, 0);

	
	// Set static H values [0..7]
	if (bLoadStaticData == TRUE)
	{
		// Set limit register
		__ASIC_WriteEngine_NoCs(iChip, iEngine, ASIC_SPI_MAP_LIMITS_LWORD, 0x82);
		__ASIC_WriteEngine_NoCs(iChip, iEngine, ASIC_SPI_MAP_LIMITS_HWORD, 0x81);
		
		// Proceed	
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x90,0xE667);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x91,0x6A09);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x92,0xAE85);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x93,0xBB67);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x94,0xF372);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x95,0x3C6E);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x96,0xF53A);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x97,0xA54F);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x98,0x527F);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x99,0x510E);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x9A,0x688C);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x9B,0x9B05);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x9C,0xD9AB);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x9D,0x1F83);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x9E,0xCD19);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0x9F,0x5BE0);	

		// STATIC [W0]
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xA8,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xA9,0x8000);
		
		// STATIC [W1]
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xAA,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xAB,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xAC,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xAD,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xAE,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xAF,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xB0,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xB1,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xB2,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xB3,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xB4,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xB5,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xB6,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xB7,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xB8,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xB9,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xBA,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xBB,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xBC,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xBD,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xBE,0x0280);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xBF,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xC0,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xC1,0x8000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xC2,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xC3,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xC4,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xC5,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xC6,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xC7,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xC8,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xC9,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xCA,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xCB,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xCC,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xCD,0x0000);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xCE,0x0100);
		__ASIC_WriteEngine_NoCs(iChip,iEngine,0xCF,0x0000);		
	
		// Load barrier offset
		__ASIC_WriteEngine_NoCs(iChip, iEngine, ASIC_SPI_MAP_BARRIER_LWORD, 0x0FF7F);
		__ASIC_WriteEngine_NoCs(iChip, iEngine, ASIC_SPI_MAP_BARRIER_HWORD, 0x0FFFF);
	}

			
	// Load H0 (MIDSTATE)
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x80,ChipMiningStatus[iChip].cMidstate[0]  | (ChipMiningStatus[iChip].cMidstate[1]  << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x81,ChipMiningStatus[iChip].cMidstate[2]  | (ChipMiningStatus[iChip].cMidstate[3]  << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x82,ChipMiningStatus[iChip].cMidstate[4]  | (ChipMiningStatus[iChip].cMidstate[5]  << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x83,ChipMiningStatus[iChip].cMidstate[6]  | (ChipMiningStatus[iChip].cMidstate[7]  << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x84,ChipMiningStatus[iChip].cMidstate[8]  | (ChipMiningStatus[iChip].cMidstate[9]  << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x85,ChipMiningStatus[iChip].cMidstate[10] | (ChipMiningStatus[iChip].cMidstate[11] << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x86,ChipMiningStatus[iChip].cMidstate[12] | (ChipMiningStatus[iChip].cMidstate[13] << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x87,ChipMiningStatus[iChip].cMidstate[14] | (ChipMiningStatus[iChip].cMidstate[15] << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x88,ChipMiningStatus[iChip].cMidstate[16] | (ChipMiningStatus[iChip].cMidstate[17] << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x89,ChipMiningStatus[iChip].cMidstate[18] | (ChipMiningStatus[iChip].cMidstate[19] << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x8A,ChipMiningStatus[iChip].cMidstate[20] | (ChipMiningStatus[iChip].cMidstate[21] << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x8B,ChipMiningStatus[iChip].cMidstate[22] | (ChipMiningStatus[iChip].cMidstate[23] << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x8C,ChipMiningStatus[iChip].cMidstate[24] | (ChipMiningStatus[iChip].cMidstate[25] << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x8D,ChipMiningStatus[iChip].cMidstate[26] | (ChipMiningStatus[iChip].cMidstate[27] << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x8E,ChipMiningStatus[iChip].cMidstate[28] | (ChipMiningStatus[iChip].cMidstate[29] << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0x8F,ChipMiningStatus[iChip].cMidstate[30] | (ChipMiningStatus[iChip].cMidstate[31] << 8));			
		
	// Load W (From JOBs)
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0xA0,ChipMiningStatus[iChip].cBlockdata[0]  | (ChipMiningStatus[iChip].cBlockdata[1]  << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0xA1,ChipMiningStatus[iChip].cBlockdata[2]  | (ChipMiningStatus[iChip].cBlockdata[3]  << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0xA2,ChipMiningStatus[iChip].cBlockdata[4]  | (ChipMiningStatus[iChip].cBlockdata[5]  << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0xA3,ChipMiningStatus[iChip].cBlockdata[6]  | (ChipMiningStatus[iChip].cBlockdata[7]  << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0xA4,ChipMiningStatus[iChip].cBlockdata[8]  | (ChipMiningStatus[iChip].cBlockdata[9]  << 8));
	__ASIC_WriteEngine_NoCs(iChip,iEngine,0xA5,ChipMiningStatus[iChip].cBlockdata[10] | (ChipMiningStatus[iChip].cBlockdata[11] << 8));

	// All data sent, now set range
	__ASIC_WriteEngine_NoCs(iChip, iEngine, ASIC_SPI_MAP_COUNTER_LOW_LWORD,  (u16)(_LowRange & 0x0FFFF));
	__ASIC_WriteEngine_NoCs(iChip, iEngine, ASIC_SPI_MAP_COUNTER_LOW_HWORD,  (u16)((_LowRange & 0x0FFFF0000) >> 16));
	__ASIC_WriteEngine_NoCs(iChip, iEngine, ASIC_SPI_MAP_COUNTER_HIGH_LWORD, (u16)(_HighRange & 0x0FFFF));
	__ASIC_WriteEngine_NoCs(iChip, iEngine, ASIC_SPI_MAP_COUNTER_HIGH_HWORD, (u16)((_HighRange & 0x0FFFF0000) >> 16));
	
	// Ignite the engine?
	if (bIgniteEngine == TRUE)
	{
		// We're all set, for this chips... Tell the engine to start
		if ((iEngine == 0))
		{
			__ASIC_WriteEngine_NoCs(iChip, iEngine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_WRITE_REGISTERS_VALID_BIT | __ActualRegister0Value));
			__ASIC_WriteEngine_NoCs(iChip, iEngine, ASIC_SPI_WRITE_REGISTER, (__ActualRegister0Value)); // Clear Write-Register Valid Bit
			
			// This info is useful for Engine activity supervision
			#if defined(__ENGINE_ENABLE_TIMESTAMPING)
				// Also mark that this engine has started processing
				GLOBAL_ENGINE_PROCESSING_START_TIMESTAMP[iChip][iEngine] = MACRO_GetTickCountRet;
				GLOBAL_ENGINE_PROCESSING_STATUS[iChip][iEngine] = TRUE;
			#endif
		}
		else
		{
			__ASIC_WriteEngine_NoCs(iChip, iEngine, ASIC_SPI_WRITE_REGISTER, (ASIC_SPI_WRITE_WRITE_REGISTERS_VALID_BIT ));
			__ASIC_WriteEngine_NoCs(iChip, iEngine, ASIC_SPI_WRITE_REGISTER, 0); // Clear Write-Register Valid Bit
			
			// This info is useful for Engine activity supervision
			#if defined(__ENGINE_ENABLE_TIMESTAMPING)
				// Also mark that this engine has started processing
				GLOBAL_ENGINE_PROCESSING_START_TIMESTAMP[iChip][iEngine] = MACRO_GetTickCountRet;
				GLOBAL_ENGINE_PROCESSING_STATUS[iChip][iEngine] = TRUE;
			#endif
		}
	}		
			
	// Deactivate the SPI
	__MCU_ASIC_Deactivate_CS();	
}

// Reset the engines...
void ASIC_reset_engine(char iChip, u16 iEngine)
{
	__MCU_ASIC_Activate_CS();			//((iChip < 8) ? (1) : (2));
	__Reset_Engine(iChip, iEngine);
	__MCU_ASIC_Deactivate_CS();			//((iChip < 8) ? (1) : (2));
}

///////////////////////////////////////////////////////////////
// A very useful macro, used to tailor code to some extent
///////////////////////////////////////////////////////////////
int ASIC_is_processing(void)
{
	unsigned char iChip;
	// Check if any chips are done...
	//int iChipsDone = 0;
	int iTotalChips = ASIC_get_chip_count();
	int iTotalChipsDone = 0;
	
	GLOBAL_LAST_ASIC_IS_PROCESSING_LATENCY = MACRO_GetTickCountRet;
	
	for (iChip = 0; iChip < TOTAL_CHIPS_INSTALLED; iChip++)
	{
		if (!CHIP_EXISTS(iChip)) continue;
		
		// Are all processors done in this engine?
		if (ASIC_are_all_engines_done(iChip) == TRUE)
		{
			iTotalChipsDone++;
		}
		else
		{
			// Still processing something
			return TRUE;
		}		
	}
	
		
	GLOBAL_LAST_ASIC_IS_PROCESSING_LATENCY = MACRO_GetTickCountRet - GLOBAL_LAST_ASIC_IS_PROCESSING_LATENCY;
		
	// Since jobs are divided equally among the engines. they will all nearly finish the same time... 
	// (Almost that is...)
	if (iTotalChips != iTotalChipsDone)
	{
		// We're not done yet...
		return TRUE;
	}
	
	// If we've reached here, it means we're not processing anymore
	return FALSE;	
}

int ASIC_is_chip_processing(char iChip)
{
	// Check if any chips are done...
	//int iChipsDone = 0;
	int iTotalChips = ASIC_get_chip_count();
	//int iTotalChipsDone = 0;
	
	if (!CHIP_EXISTS(iChip)) return FALSE;
		
	// Are all processors done in this engine?
	if (ASIC_are_all_engines_done(iChip) == TRUE)
	{
		// We're no longer processing
		return FALSE;
	}
	else
	{
		// Still processing something
		return TRUE;
	}		
}

int	 ASIC_is_engine_processing(char iChip, u16 iEngine)
{
	int bRetVal;
	if (!CHIP_EXISTS(iChip)) return FALSE;
	if (!IS_PROCESSOR_OK(iChip, iEngine)) return FALSE;
	__MCU_ASIC_Activate_CS();			//((iChip < 8) ? (1) : (2));
	bRetVal =  ((__ARM_SC_ReadData(iChip, iEngine, ASIC_SPI_READ_STATUS_REGISTER) & ASIC_SPI_READ_STATUS_BUSY_BIT) == ASIC_SPI_READ_STATUS_BUSY_BIT);
	__MCU_ASIC_Deactivate_CS();			//((iChip < 8) ? (1) : (2));
	return bRetVal;	
}

int ASIC_get_chip_count(void)
{
	int iActualChipCount;
	int x_chip;
	int iRepeating;
	u32 dwDelay;
	//u16	wTestRegisterValue;
	// This function WILL NOT CHANGE THE MAP IF THE ENUMERATION HAS BEEN PERFORMED BEFORE!
	if ((__internal_global_iChipCount != 0))
		return __internal_global_iChipCount;
						
	// We haven't, so we need to read their register (one by one) to find which ones exist.
	iActualChipCount = 0;
	
	//for (x_chip = 0; x_chip < TOTAL_CHIPS_INSTALLED; x_chip++)
	for (x_chip = 3; x_chip < 4; x_chip++)
	{
		// Activate CS# of ASIC engines
		__MCU_ASIC_Activate_CS();			//((x_chip < 8) ? (1) : (2));		
		
		#if defined(DECOMISSION_CHIP_15)
			if (x_chip == 15) { __chip_existence_map[x_chip] = 0; continue; }
		#endif		
		
		#if defined(DECOMISSION_CHIP_14)
			if (x_chip == 14) { __chip_existence_map[x_chip] = 0; continue; }
		#endif		
		
		#if defined(DECOMISSION_CHIP_13)
			if (x_chip == 13) { __chip_existence_map[x_chip] = 0; continue; }
		#endif		
		
		#if defined(DECOMISSION_CHIP_12)
			if (x_chip == 12) { __chip_existence_map[x_chip] = 0; continue; }
		#endif		
		
		#if defined(DECOMISSION_CHIP_11)
			if (x_chip == 11) { __chip_existence_map[x_chip] = 0; continue; }
		#endif		
		
		#if defined(DECOMISSION_CHIP_10)
			if (x_chip == 10) { __chip_existence_map[x_chip] = 0; continue; }
		#endif		
		
		#if defined(DECOMISSION_CHIP_9)
			if (x_chip == 9) { __chip_existence_map[x_chip] = 0; continue; }
		#endif		
		
		#if defined(DECOMISSION_CHIP_8)
			if (x_chip == 8) { __chip_existence_map[x_chip] = 0; continue; }
		#endif
			
		#if defined(DECOMISSION_CHIP_7)
			if (x_chip == 7) { __chip_existence_map[x_chip] = 0; continue; }
		#endif
		
		#if defined(DECOMISSION_CHIP_6)
			if (x_chip == 6) { __chip_existence_map[x_chip] = 0; continue; }
		#endif		
		
		#if defined(DECOMISSION_CHIP_5)
			if (x_chip == 5) { __chip_existence_map[x_chip] = 0; continue; }
		#endif
		
		#if defined(DECOMISSION_CHIP_3)
			if (x_chip == 3) { __chip_existence_map[x_chip] = 0; continue; }		
		#endif
		
		#if defined(DECOMISSION_CHIP_4)
			if (x_chip == 4) { __chip_existence_map[x_chip] = 0; continue; }
		#endif
			
		#if defined(DECOMISSION_CHIP_2)
			if (x_chip == 2) { __chip_existence_map[x_chip] = 0; continue; }
		#endif
		
		#if defined(DECOMISSION_CHIP_1)
			if (x_chip == 1) { __chip_existence_map[x_chip] = 0; continue; }
		#endif
		
		#if defined(DECOMISSION_CHIP_0)
			if (x_chip == 0) { __chip_existence_map[x_chip] = 0; continue; }
		#endif
		
		// Proceed
		//wTestRegisterValue = 0x0F81F + x_chip;
		for (iRepeating = 0; iRepeating < 100; iRepeating++)
		{
			//because of SPI read back value though write engine command at the physical chip, 
			//we have to modify code. maybe affect the following code.  It is different from datasheet!!!
			__ASIC_WriteEngine_NoCs(x_chip, 0, ASIC_SPI_READ_TEST_REGISTER,0x0F81F + x_chip);
			if (__ASIC_ReadEngine_NoCs(x_chip,0, ASIC_SPI_READ_TEST_REGISTER) == 0x0F81F + x_chip)
			{
				iActualChipCount++;
				
				#if defined(DO_NOT_USE_ENGINE_ZERO)
					__chip_existence_map[x_chip] = 0x0FFFF & (~(0x01));
				#else
					__chip_existence_map[x_chip] = 0x0FFFF;
				#endif
				
				break;
			}
			dwDelay = 0x00ff;
			while((dwDelay--) != 0)
			{
			}
			//__ASIC_ReadEngine_NoCs(x_chip,0, ASIC_SPI_READ_TEST_REGISTER); //read op seems can not remove
			//__ASIC_WriteEngine_NoCs(x_chip, 0, 0xfff0,0x0FFFF);		//write to different reg will affect read back value
		}
		
		// Deactivate CS# of ASIC engines
		__MCU_ASIC_Deactivate_CS(); 			//((x_chip < 8) ? (1) : (2));				
	}
	

	
	__internal_global_iChipCount = iActualChipCount;
	return iActualChipCount;	
}

int ASIC_does_chip_exist(unsigned int iChipIndex)
{
	// Does this chip exist?
	return CHIP_EXISTS(iChipIndex);
}

u16 __ASIC_WriteEngine(u8 iChip, u16 iEngine, u8 iAddress, u16 iData16Bit)
{
	u16 wReadData;
	// We issue the SPI command
	//MCU_SC_SetAccess();
	//MCU_SC_WriteData(iChip, iEngine, iAddress, iData16Bit);
	//__AVR32_SC_WriteData(iChip, iEngine, iAddress, iData16Bit);
	//ComTransmitData(COM2, "W",1);
	__MCU_ASIC_Activate_CS();
	wReadData = __ARM_SC_WriteData(iChip, iEngine, iAddress, iData16Bit);
	__MCU_ASIC_Deactivate_CS();
	return wReadData;
}

u16 __ASIC_WriteEngine_NoCs(u8 iChip, u16 iEngine, u8 iAddress, u16 iData16Bit)
{
	// We issue the SPI command
	//MCU_SC_SetAccess();
	//MCU_SC_WriteData(iChip, iEngine, iAddress, iData16Bit);
	//__AVR32_SC_WriteData(iChip, iEngine, iAddress, iData16Bit);
	//ComTransmitData(COM2, "w",1);
	return __ARM_SC_WriteData(iChip, iEngine, iAddress, iData16Bit);
}


u16 __ASIC_ReadEngine(u8 iChip, u16 iEngine, u8 iAddress)
{
	u16 wTemp;
	// We issue the SPI command
	//MCU_SC_SetAccess();
	// return MCU_SC_ReadData(iChip, iEngine, (unsigned char)iAddress);
	//ComTransmitData(COM2, "G",1);
	__MCU_ASIC_Activate_CS();
	wTemp = __ARM_SC_ReadData(iChip, iEngine, iAddress);
	__MCU_ASIC_Deactivate_CS();
	return wTemp;
}

u16 __ASIC_ReadEngine_NoCs(u8 iChip, u16 iEngine, u8 iAddress)
{
	u16 wTemp;
	// We issue the SPI command
	//MCU_SC_SetAccess();
	// return MCU_SC_ReadData(iChip, iEngine, (unsigned char)iAddress);
	//ComTransmitData(COM2, "g",1);
	wTemp = __ARM_SC_ReadData(iChip, iEngine, iAddress);
	return wTemp;
}



u16 CHIP_EXISTS(u8 x)
{
	u16 cResult;
	cResult = __chip_existence_map[x];		//16bit correlated to 16 engines
													//0 means bad engine, cResult=0 means full chip bad or chip not exist

	return cResult;
}

u16 IS_PROCESSOR_OK(u8 xchip, u16 yengine)
{
	u16 dResult;
	dResult = (__chip_existence_map[xchip] & (1 << yengine));

	return dResult;
}



