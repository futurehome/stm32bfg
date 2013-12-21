/*
  ******************************************************************************
  * @file    bf_peripheral_timer.c
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   timer routines prototypes
  ******************************************************************************
*/

#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
//#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_endp.h"
#include "usb_core.h"
#include "bf_general.h"


#include "bf_general.h"

#include "bf_peripheral_uart.h"
#include "bf_simu_platform.h"

#include "bf_peripheral_adc.h"
#include "bf_peripheral_timer.h"
#include "std_defs.h"

u32 dwSystemTickCounterHighWord = 0;

SOFT_TIMER SoftTimer0;
SOFT_TIMER SoftTimer1;
SOFT_TIMER ChipSoftTimer[TOTAL_CHIPS_INSTALLED];


/**
  * @brief  Initializes the timer1.
  * @param  None
  * @retval None
  */
void BF_Timer1_LowLevel_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	AFIO_TypeDef * AFIO_RegisterGroup;

	/*Timer1 Periph clock enable */
	RCC_APB2PeriphClockCmd((BF_TIMER1_CLK | RCC_APB2Periph_AFIO), ENABLE); 	//enable alter I/O for clock out

	/*Timer1 GPIO Periph clock enable */
	RCC_APB2PeriphClockCmd(BF_TIMER1_GPIO_CLK, ENABLE);

	/*!< Configure timer1 output pins: CH1N, PE8 */
	GPIO_InitStructure.GPIO_Pin = BF_TIMER1_CH1N_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//GPIO alter function output
	GPIO_Init(BF_TIMER1_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure timer1 output pin: CH1, PE9 */
	GPIO_InitStructure.GPIO_Pin = BF_TIMER1_CH1_PIN;
	GPIO_Init(BF_TIMER1_GPIO_PORT, &GPIO_InitStructure);

	AFIO_RegisterGroup = AFIO;
	AFIO_RegisterGroup->MAPR &= TIMER1_AFIO_REMAP_MASK;
	AFIO_RegisterGroup->MAPR |= TIMER1_AFIO_REMAP_FULL;			//Timer1 full remap to pin PE8/PE9

}

void BF_Timer1_Init(void)
{
	TIM_TypeDef * Timer1_RegisterGroup;

	Timer1_RegisterGroup = TIM1;

	BF_Timer1_LowLevel_Init();

	BF_Timer1UnlockRegister();

	Timer1_RegisterGroup->PSC = TIMER1_PRESCALER_VALUE;		//counter in clock is 72MHz/9 = 8MHz
	Timer1_RegisterGroup->ARR = TIMER1_AUTO_RELOAD_VALUE;	//counter overflow freq is 8MHz/8 = 1MHz

	Timer1_RegisterGroup->CR2 = 0;
	Timer1_RegisterGroup->SMCR = 0;			//slave disable
	Timer1_RegisterGroup->DIER = 0;			//interrupt disable
	Timer1_RegisterGroup->EGR = 0;			//event generate null
	Timer1_RegisterGroup->CCMR1 = TIMER1_CCMR_OCM_TOGGLE | TIMER1_CCMR_CC1S_OUTPUT;
	Timer1_RegisterGroup->CCMR2 = 0;
	Timer1_RegisterGroup->CCER = TIMER1_CCER_CC1E_OUTPUT_ENABLE | TIMER1_CCER_CC1NE_COMPLEMENTARY_EN
									|TIMER1_CCER_CC1P_POLARITY_HIGH |TIMER1_CCER_CC1NP_POLARITY_HIGH;
	Timer1_RegisterGroup->RCR = 0;			//repetition 0
	Timer1_RegisterGroup->CCR1 = 1;			//dummy data within counter value
	Timer1_RegisterGroup->CCR2 = 0;
	Timer1_RegisterGroup->CCR3 = 0;
	Timer1_RegisterGroup->CCR4 = 0;
	Timer1_RegisterGroup->BDTR = T1_BDTR_DTG_DEAD_TIME | T1_BDTR_MOE_OUTPUT_ENABLE;
	Timer1_RegisterGroup->DCR = 0;			//DMA disable
	Timer1_RegisterGroup->DMAR = 0;			//DMA address useless
	Timer1_RegisterGroup->CR1 = T1_CR1_COUNT_DOWN | T1_CR1_COUNTER_ENABLE;

	BF_Timer1LockRegister();
}

void BF_Timer1UnlockRegister(void)
{
	TIM_TypeDef * Timer1_RegisterGroup;
	Timer1_RegisterGroup = TIM1;

	Timer1_RegisterGroup->BDTR &= TIMER1_UNLOCK_REGISTER;
}

void BF_Timer1LockRegister(void)
{
	TIM_TypeDef * Timer1_RegisterGroup;
	Timer1_RegisterGroup = TIM1;

	Timer1_RegisterGroup->BDTR |= TIMER1_LOCK_REGISTER_LEVEL_3;
}

void BF_Timer2_LowLevel_Init(void)
{
	/*Timer2  Periph clock enable */
	RCC_APB1PeriphClockCmd(BF_TIMER2_CLK, ENABLE);
}

//Setup Timer2 as systemtick ,   every 10mS will generate a flag
void BF_Timer2_Init(void)
{
		TIM_TypeDef * Timer2_RegisterGroup;

	Timer2_RegisterGroup = TIM2;

	BF_Timer2_LowLevel_Init();

	//BF_Timer1UnlockRegister();

	Timer2_RegisterGroup->PSC = T2_PRESCALER_VALUE;		//counter in clock is 36MHz/720 = 50KHz
	Timer2_RegisterGroup->ARR = T2_AUTO_RELOAD_VALUE;	//counter overflow freq is 50KHz/500 = 100Hz, 10mS

	Timer2_RegisterGroup->CR2 = 0;
	Timer2_RegisterGroup->SR = 0;
	Timer2_RegisterGroup->SMCR = 0;			//slave disable
	Timer2_RegisterGroup->DIER = 0x01;			//only update interrupt enable
	Timer2_RegisterGroup->EGR = 0;			//event generate null
	Timer2_RegisterGroup->CCMR1 = 0;
	Timer2_RegisterGroup->CCMR2 = 0;
	Timer2_RegisterGroup->CCER = 0;
	Timer2_RegisterGroup->RCR = 0;			//repetition 0
	Timer2_RegisterGroup->CCR1 = 0xFFFE;			//dummy data within counter value
	Timer2_RegisterGroup->CCR2 = 0xFFFE;
	Timer2_RegisterGroup->CCR3 = 0xFFFE;
	Timer2_RegisterGroup->CCR4 = 0xFFFE;
	Timer2_RegisterGroup->BDTR = 0;
	Timer2_RegisterGroup->DCR = 0;			//DMA disable
	Timer2_RegisterGroup->DMAR = 0;			//DMA address useless
	Timer2_RegisterGroup->CR1 = T1_CR1_COUNT_DOWN | T1_CR1_COUNTER_ENABLE 
									 | T1_CR1_UPDATE_REQUEST_SOURCE_OVERFLOW;
}

/*
	Timer2 works as system tick, will start ADC, operate soft timer, refresh Temperature/FAN status etc
*/
void Timer2_ISR(void)
{
	TIM_TypeDef * Timer2_RegisterGroup;
	u16 wTimer2SR;
	u8	cChip;

	Timer2_RegisterGroup = TIM2;

	wTimer2SR = Timer2_RegisterGroup->SR;
	if(wTimer2SR & TIM_SR_UIF)
	{
		Timer2_RegisterGroup->SR &= (~TIM_SR_UIF);		//clear interrupt flag
		if(~((*(u32*)DMA1_Channel1_BASE) & 0x00000001))
		{
			BF_ADC_Start();			//restart ADC
		}
	}

	if(SoftTimer0.cTimerRun)
	{
		if(SoftTimer0.dwTimerCount < SoftTimer0.dwTimeOut)
		{
			SoftTimer0.dwTimerCount ++;
			SoftTimer0.cTimerAlarm = FALSE;
		}
		else
		{
			SoftTimer0.cTimerRun = FALSE;
			SoftTimer0.cTimerAlarm = TRUE;
		}
	}
	/*
	if(SoftTimer1.cTimerRun)
	{
		if(SoftTimer1.dwTimerCount < SoftTimer1.dwTimeOut)
		{
			SoftTimer1.dwTimerCount ++;
			SoftTimer1.cTimerAlarm = FALSE;
		}
		else
		{
			SoftTimer1.cTimerRun = FALSE;
			SoftTimer1.cTimerAlarm = TRUE;
		}
	}*/

	for(cChip=0; cChip<TOTAL_CHIPS_INSTALLED; cChip++)
	{
		if(ChipSoftTimer[cChip].cTimerRun)
		{
			if(ChipSoftTimer[cChip].dwTimerCount < ChipSoftTimer[cChip].dwTimeOut)
			{
				ChipSoftTimer[cChip].dwTimerCount ++;
				ChipSoftTimer[cChip].cTimerAlarm = FALSE;
			}
			else
			{
				ChipSoftTimer[cChip].cTimerRun = FALSE;
				ChipSoftTimer[cChip].cTimerAlarm = TRUE;
			}
		}
	}
}



/**
  * @brief  Initializes the timer3.
  * @param  None
  * @retval None
  */
void BF_Timer3_LowLevel_Init(void)
{
	/*Timer3 Periph clock enable */
	RCC_APB1PeriphClockCmd(BF_TIMER3_CLK, ENABLE); 	//enable alter I/O for clock out
}


void BF_Timer3_Init(void)
{
	TIM_TypeDef * Timer3_RegisterGroup;

	Timer3_RegisterGroup = TIM3;

	BF_Timer3_LowLevel_Init();

	Timer3_RegisterGroup->PSC = T3_PRESCALER_VALUE;		//counter in clock is 36MHz/(8+1) = 4MHz
	Timer3_RegisterGroup->ARR = T3_AUTO_RELOAD_VALUE;	//counter overflow freq is 4MHz/(3+1)= 1MHz

	Timer3_RegisterGroup->CR2 = 0;
	Timer3_RegisterGroup->SMCR = 0;			//slave disable
	Timer3_RegisterGroup->DIER = 0x01;			//only update interrupt enable
	Timer3_RegisterGroup->EGR = 0;			//event generate null
	Timer3_RegisterGroup->CCMR1 = 0;
	Timer3_RegisterGroup->CCMR2 = 0;
	Timer3_RegisterGroup->CCER = 0;
	Timer3_RegisterGroup->RCR = 0;			//repetition 0
	Timer3_RegisterGroup->CCR1 = 0xFFF0;			//dummy data within counter value
	Timer3_RegisterGroup->CCR2 = 0xFFF0;
	Timer3_RegisterGroup->CCR3 = 0xFFF0;
	Timer3_RegisterGroup->CCR4 = 0xFFF0;
	Timer3_RegisterGroup->BDTR = 0;
	Timer3_RegisterGroup->DCR = 0;			//DMA disable
	Timer3_RegisterGroup->DMAR = 0;			//DMA address useless
	//Timer3_RegisterGroup->CR1 = T1_CR1_COUNT_DOWN | T1_CR1_COUNTER_ENABLE
	//								| T1_CR1_UPDATE_REQUEST_SOURCE_OVERFLOW;
	Timer3_RegisterGroup->CR1 = T1_CR1_COUNTER_ENABLE;

}

void Timer3_ISR(void)
{
	TIM_TypeDef * Timer3_RegisterGroup;
	u16 wTimer3SR;

	Timer3_RegisterGroup = TIM3;

	wTimer3SR = Timer3_RegisterGroup->SR;
	if(wTimer3SR & TIM_SR_UIF)
	{
		Timer3_RegisterGroup->SR &= (~TIM_SR_UIF);		//clear interrupt flag
		dwSystemTickCounterHighWord += 0x00010000;
	}
	
}

//Usage of SoftTimer0 is for USB delay to receive mid-state data
void StartSoftTimer0(u32 dwTimeOut)		//SoftTimer0 using Timer2 tick, 10mS
{
	SoftTimer0.cTimerAlarm = FALSE;
	SoftTimer0.dwTimeOut = dwTimeOut;
	SoftTimer0.dwTimerCount = 0;
	SoftTimer0.cTimerRun = TRUE;
}

void ClearSoftTimer0(void)
{
	SoftTimer0.cTimerAlarm = FALSE;
	SoftTimer0.dwTimeOut = 0xFFFFFFF0;		//set time out Max
	SoftTimer0.dwTimerCount = 0;
	SoftTimer0.cTimerRun = FALSE;			//timer stop
}

//Usage of SoftTimer1 is adding some dalay to get nonce result for BF engine, after start calculating
//By roughly test, need about 15s to get the result for BF engine from 0x00000000 to 0xFFFFFFFF, FREQ slect is 7
void StartSoftTimer1(u32 dwTimeOut)		//SoftTimer0 using Timer2 tick, 5mS  ???
{
	SoftTimer1.cTimerAlarm = FALSE;
	SoftTimer1.dwTimeOut = dwTimeOut;
	SoftTimer1.dwTimerCount = 0;
	SoftTimer1.cTimerRun = TRUE;
}

void ClearSoftTimer1(void)
{
	SoftTimer1.cTimerAlarm = FALSE;
	SoftTimer1.dwTimeOut = 0xFFFFFFF0;		//set time out Max
	SoftTimer1.dwTimerCount = 0;
	SoftTimer1.cTimerRun = FALSE;			//timer stop
}

void StartSoftTimerEx(u8 cChip, u32 dwTimeOut)		//SoftTimer0 using Timer2 tick, 5mS  ???
{
	ChipSoftTimer[cChip].cTimerAlarm = FALSE;
	ChipSoftTimer[cChip].dwTimeOut = dwTimeOut;
	ChipSoftTimer[cChip].dwTimerCount = 0;
	ChipSoftTimer[cChip].cTimerRun = TRUE;
}

void ClearSoftTimerEx(u8 cChip)
{
	ChipSoftTimer[cChip].cTimerAlarm = FALSE;
	ChipSoftTimer[cChip].dwTimeOut = 0xFFFFFFF0;		//set time out Max
	ChipSoftTimer[cChip].dwTimerCount = 0;
	ChipSoftTimer[cChip].cTimerRun = FALSE;			//timer stop
}


