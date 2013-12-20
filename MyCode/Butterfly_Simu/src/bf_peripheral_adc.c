/*
  ******************************************************************************
  * @file    bf_peripheral_adc.c
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   adc routines prototypes
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

u16 wAdcResult[BF_ADC_CHANNEL_NUMBER];
u16 wAdcFilteredResult[BF_ADC_CHANNEL_NUMBER];


/**
  * @brief  Initializes the ADC.
  * @param  None
  * @retval None
  */
void BF_ADC_LowLevel_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_TypeDef	* RCC_RegisterGroup;

	RCC_RegisterGroup = RCC;

	//set ADC convert clock = PLCK2/6, 72MHz/8 = 9MHz
	RCC_RegisterGroup->CFGR &= ADC_RCC_CLK_MASK;
	RCC_RegisterGroup->CFGR |= ADC_RCC_CLK_DIV8;

	/*!< ADC1 Periph clock enable */
	RCC_APB2PeriphClockCmd(BF_ADC_CLK, ENABLE);

	/*!< ADC GPIO Periph clock enable */
	RCC_APB2PeriphClockCmd(BF_ADC_CH10_GPIO_CLK | BF_ADC_CH11_GPIO_CLK
								| BF_ADC_CH12_GPIO_CLK | BF_ADC_CH13_GPIO_CLK, ENABLE);

	/*!< Configure ADC pins: ch10, PC0 */
	GPIO_InitStructure.GPIO_Pin = BF_ADC_CH10_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(BF_ADC_GPIOC_PORT, &GPIO_InitStructure);

	/*!< Configure ADC pins: ch11, PC1 */
	GPIO_InitStructure.GPIO_Pin = BF_ADC_CH11_PIN;
	GPIO_Init(BF_ADC_GPIOC_PORT, &GPIO_InitStructure);

	/*!< Configure ADC pin: ch12, PC2 */
	GPIO_InitStructure.GPIO_Pin = BF_ADC_CH12_PIN;
	GPIO_Init(BF_ADC_GPIOC_PORT, &GPIO_InitStructure); 

	/*!< Configure ADC pin: ch13, PC3 */
	GPIO_InitStructure.GPIO_Pin = BF_ADC_CH13_PIN;
	GPIO_Init(BF_ADC_GPIOC_PORT, &GPIO_InitStructure); 

	/*!< Configure ADC pin: ch0, PA0 */
	GPIO_InitStructure.GPIO_Pin = BF_ADC_CH0_PIN;		//bf_temp1
	GPIO_Init(BF_ADC_GPIOA_PORT, &GPIO_InitStructure); 

	/*!< Configure ADC pin: ch1, PA1 */
	GPIO_InitStructure.GPIO_Pin = BF_ADC_CH1_PIN;		//bf_temp0
	GPIO_Init(BF_ADC_GPIOA_PORT, &GPIO_InitStructure); 
}

void BF_ADC_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	u8 i;
	
	BF_ADC_LowLevel_Init();
	
	for(i=0; i<BF_ADC_CHANNEL_NUMBER; i++)
	{
		wAdcFilteredResult[i] = 2;
	}

	ADC_DeInit(BF_ADC);

	/* BF ADC Init value */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;		//group of multchannels adc should using scan and DMA
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = BF_ADC_CHANNEL_NUMBER;
	ADC_InitStructure.ADC_DMA = ADC_DMA_ENABLE;

	ADC_Init(BF_ADC, &ADC_InitStructure);

	//ADC_ITConfig(BF_ADC, ADC_IT_EOC, ENABLE);

	ADC_RegularChannelConfig(BF_ADC, ADC_Channel_10, 1, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(BF_ADC, ADC_Channel_11, 2, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(BF_ADC, ADC_Channel_12, 3, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(BF_ADC, ADC_Channel_13, 4, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(BF_ADC, ADC_Channel_0, 5, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(BF_ADC, ADC_Channel_1, 6, ADC_SampleTime_28Cycles5);

	ADC1_DMA1_Hook();

	ADC_Cmd(BF_ADC, ENABLE);
}

void BF_ADC_Start(void)
{
	(*(u32*)(DMA1_Channel1_BASE + 0x0004)) = BF_ADC_CHANNEL_NUMBER;
	ADC_Cmd(BF_ADC, ENABLE);
	DMA_Cmd(BF_ADC_DMA_CHANNEL,ENABLE);
	ADC_SoftwareStartConvCmd(BF_ADC, ENABLE);
}

void ADC1_DMA1_Hook(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	  
	RCC_AHBPeriphClockCmd(BF_ADC_DMA_CLK, ENABLE);

	/* Initialize the DMA_PeripheralBaseAddr member */
	DMA_InitStructure.DMA_PeripheralBaseAddr = BF_ADC_REG_DR;
	/* Initialize the DMA_MemoryBaseAddr member */
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)wAdcResult;
	/* Initialize the DMA_PeripheralInc member */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	/* Initialize the DMA_MemoryInc member */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	/* Initialize the DMA_PeripheralDataSize member */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	/* Initialize the DMA_MemoryDataSize member */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	/* Initialize the DMA_Mode member */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	/* Initialize the DMA_Priority member */
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	/* Initialize the DMA_M2M member */
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	/* Initialize the DMA_DIR member */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

	/* Initialize the DMA_BufferSize member */
	DMA_InitStructure.DMA_BufferSize = BF_ADC_CHANNEL_NUMBER;

	DMA_DeInit(BF_ADC_DMA_CHANNEL);

	DMA_Init(BF_ADC_DMA_CHANNEL, &DMA_InitStructure);
	(*(u32*)DMA1_Channel1_BASE) |= DMA1_CCR_TCIE ;

	DMA_Cmd(BF_ADC_DMA_CHANNEL,DISABLE);

	
}

void ADC1_DMA1_ISR(void)
{
	DMA_TypeDef * DMA1_RegisterBase;
	u32	dwDma1Isr;
	u8 i;
	u32 dwTemp;
	
	DMA1_RegisterBase = DMA1;
	dwDma1Isr = DMA1_RegisterBase->ISR;

	if(dwDma1Isr & DMA1_FLAG_TC1)
	{
		DMA1_RegisterBase->IFCR |= DMA1_IT_TC1;		//clear DMA TC1 int flag
		for(i=0; i<BF_ADC_CHANNEL_NUMBER; i++)
		{
			dwTemp = wAdcFilteredResult[i] + wAdcResult[i];
			dwTemp += (wAdcFilteredResult[i] << 1);
			wAdcFilteredResult[i] = (u16)(dwTemp >> 2);
		}
		ADC_Cmd(BF_ADC, DISABLE);
		DMA_Cmd(BF_ADC_DMA_CHANNEL,DISABLE);
	}
}



