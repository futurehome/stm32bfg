/*
  ******************************************************************************
  * @file    bf_peripheral_adc.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   ADC routines prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BF_PERIPHERAL_ADC_H
#define __BF_PERIPHERAL_ADC_H

#define ADC1_DMA1_ISR DMA1_Channel1_IRQHandler

#define BF_ADC_CHANNEL_NUMBER 6

#define ADC_RCC_CLK_DIV2	0x00000000
#define ADC_RCC_CLK_DIV4	0x00004000
#define	ADC_RCC_CLK_DIV6	0x00008000
#define	ADC_RCC_CLK_DIV8	0x0000C000
#define	ADC_RCC_CLK_MASK	0xFFFF3FFF

#define DMA1_CCR_TCIE		0x0002
#define	DMA1_CCR_EN			0x0001

void BF_ADC_LowLevel_Init(void);
void BF_ADC_Init(void);
void BF_ADC_Start(void);
void ADC1_DMA1_Hook(void);
void ADC1_DMA1_ISR(void);
void ADC1_DMA1_ISR(void);





#endif /* __BF_PERIPHERAL_ADC_H */


