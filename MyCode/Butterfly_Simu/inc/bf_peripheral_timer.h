/*
  ******************************************************************************
  * @file    bf_peripheral_timer.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   ADC routines prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BF_PERIPHERAL_TIMER_H
#define __BF_PERIPHERAL_TIMER_H

#define Timer2_ISR			TIM2_IRQHandler
#define Timer3_ISR 			TIM3_IRQHandler

#define TIMER1_AFIO_REMAP_MASK		0xFFFFFF3F
#define	TIMER1_AFIO_REMAP_PARTIAL	0x00000040
#define	TIMER1_AFIO_REMAP_FULL		0x000000C0

#define TIMER1_PRESCALER_VALUE		8
#define	TIMER1_AUTO_RELOAD_VALUE	7			//counter overflow will be 72MHz/(8+1)/(7+1) = 1MHz

#define TIMER1_CCMR_OCM_TOGGLE			0x0030
#define	TIMER1_CCMR_CC1S_OUTPUT			0x0000		//CC1 channel mode is output

#define TIMER1_CCER_CC1NP_POLARITY_LOW	0x0008
#define	TIMER1_CCER_CC1NP_POLARITY_HIGH	0x0000
#define TIMER1_CCER_CC1NE_COMPLEMENTARY_EN	0x0004
#define TIMER1_CCER_CC1P_POLARITY_HIGH	0x0000
#define TIMER1_CCER_CC1P_POLARITY_LOW	0x0002
#define TIMER1_CCER_CC1E_OUTPUT_ENABLE	0x0001

#define TIMER1_UNLOCK_REGISTER				0xFCFF
#define TIMER1_LOCK_REGISTER_LEVEL_1		0x0100
#define	TIMER1_LOCK_REGISTER_LEVEL_2		0x0200
#define	TIMER1_LOCK_REGISTER_LEVEL_3		0x0300

#define T1_BDTR_MOE_OUTPUT_ENABLE			0x8000
#define T1_BDTR_DTG_DEAD_TIME				0x0000

#define T1_CR1_COUNT_DOWN					0x0010
#define T1_CR1_COUNTER_ENABLE				0x0001
#define	T1_CR1_UPDATE_REQUEST_SOURCE_OVERFLOW	0x0004
#define T1_CR1_UDIS_UPDATE_DISABLE			0x0002

/*	Timer2 Register and some setup value */
#define	T2_PRESCALER_VALUE				720			//Counter Clock-in = Rcc-source / 720, if in=36MHz, Counter-in =  50KHz, 20uS	
#define T2_AUTO_RELOAD_VALUE			500			//set sys-stick = 50KHz/50, 1mS

/*	Timer2 Register and some setup value */
#define T3_PRESCALER_VALUE				35		//timer3 works as system tick, 1us
#define T3_AUTO_RELOAD_VALUE			0xFFFF

extern u32 dwSystemTickCounterHighWord;
//#define MACRO_GetTickCountRet dwSystemTickCounter
#define MACRO_GetTickCountRet (dwSystemTickCounterHighWord | (*(u16*)((uint32_t)(0x40000000 + 0x0424))))	//T2 Counter, 16bit



typedef struct
{ 
	u8 cTimerRun;		//soft timer control,  TRUE=run; FALSE = off
	u32 dwTimeOut;		//target time out value, time unit is Timer2 tick, 10mS
	u32 dwTimerCount;	//current timer count
	u8 cTimerAlarm;		//timer count reach time out value, and timer is off
}SOFT_TIMER;


void BF_Timer1_LowLevel_Init(void);
void BF_Timer1_Init(void);
void BF_Timer1UnlockRegister(void);
void BF_Timer1LockRegister(void);
void BF_Timer2_LowLevel_Init(void);
void BF_Timer2_Init(void);

void Timer2_ISR(void);

void BF_Timer3_LowLevel_Init(void);
void BF_Timer3_Init(void);
void Timer3_ISR(void);
void StartSoftTimer0(u32 dwTimeOut);
void ClearSoftTimer0(void);
void StartSoftTimer1(u32 dwTimeOut);
void ClearSoftTimer1(void);
void StartSoftTimerEx(u8 cChip, u32 dwTimeOut);
void ClearSoftTimerEx(u8 cChip);














#endif /* __BF_PERIPHERAL_ADC_H */


