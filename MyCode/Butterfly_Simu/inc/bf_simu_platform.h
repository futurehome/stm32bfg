/*
  ******************************************************************************
  * @file    bf_simu_platform.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   usb endpoint routines prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BF_SIMU_PLATFORM_H
#define __BF_SIMU_PLATFORM_H

 #include "stm32f10x.h"
 #include "bf_peripheral_uart.h"



#define FT232R_VENDOR_REQ_GET_90 0x90
#define FT232R_VENDOR_REQ_GET_05 0x05

#define FT232R_VENDOR_REQ_SET_00 0x00
#define FT232R_VENDOR_REQ_SET_01 0x01
#define FT232R_VENDOR_REQ_SET_02 0x02
#define FT232R_VENDOR_REQ_SET_03 0x03
#define FT232R_VENDOR_REQ_SET_04 0x04
#define FT232R_VENDOR_REQ_SET_06 0x06
#define FT232R_VENDOR_REQ_SET_09 0x09


 

/**  USB port pin
*/
#define USB_DISCONNECT                      GPIOA  
#define USB_DISCONNECT_PIN                  GPIO_Pin_8
#define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOA



/** @addtogroup BF_SIMU_LOW_LEVEL_COM
  * @{
  */


/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define BF_COM1_BASE                   USART1
#define BF_COM1_CLK                    RCC_APB2Periph_USART1
#define BF_COM1_TX_PIN                 GPIO_Pin_9
#define BF_COM1_TX_GPIO_PORT           GPIOA
#define BF_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define BF_COM1_RX_PIN                 GPIO_Pin_10
#define BF_COM1_RX_GPIO_PORT           GPIOA
#define BF_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define BF_COM1_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port2, connected to USART2 (USART2 pins remapped on GPIOD)
 */ 
#define BF_COM2_BASE                   0x40004400
#define BF_COM2_CLK                    RCC_APB1Periph_USART2
#define BF_COM2_TX_PIN                 GPIO_Pin_5
#define BF_COM2_TX_GPIO_PORT           GPIOD
#define BF_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define BF_COM2_RX_PIN                 GPIO_Pin_6
#define BF_COM2_RX_GPIO_PORT           GPIOD
#define BF_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define BF_COM2_IRQn                   USART2_IRQn

/**
  * @brief   SPI Interface pins
  */  
#define BF_SPI                       SPI1                        //0x40013000
#define BF_SPI_CLK                   RCC_APB2Periph_SPI1
#define BF_SPI_SCK_PIN               GPIO_Pin_5                  /* PA.05 */
#define BF_SPI_SCK_GPIO_PORT         GPIOA                       /* GPIOA */
#define BF_SPI_SCK_GPIO_CLK          RCC_APB2Periph_GPIOA
#define BF_SPI_MISO_PIN              GPIO_Pin_6                  /* PA.06 */
#define BF_SPI_MISO_GPIO_PORT        GPIOA                       /* GPIOA */
#define BF_SPI_MISO_GPIO_CLK         RCC_APB2Periph_GPIOA
#define BF_SPI_MOSI_PIN              GPIO_Pin_7                  /* PA.07 */
#define BF_SPI_MOSI_GPIO_PORT        GPIOA                       /* GPIOA */
#define BF_SPI_MOSI_GPIO_CLK         RCC_APB2Periph_GPIOA
#define BF_SPI_CS_PIN                    GPIO_Pin_4                  /* PA.04 */
#define BF_SPI_CS_GPIO_PORT              GPIOA                       /* GPIOA */
#define BF_SPI_CS_GPIO_CLK               RCC_APB2Periph_GPIOA 

/*BF Done Pin */
#define BF_DONE0_PIN	GPIO_Pin_12
#define	BF_DONE0_GPIO_PORT	GPIOE
#define BF_DONE0_GPIO_CLK	RCC_APB2Periph_GPIOE
#define BF_DONE1_PIN		GPIO_Pin_13
#define	BF_DONE1_GPIO_PORT	GPIOE
#define BF_DONE1_GPIO_CLK	RCC_APB2Periph_GPIOE
#define BF_DONE2_PIN		GPIO_Pin_14
#define	BF_DONE2_GPIO_PORT	GPIOE
#define BF_DONE2_GPIO_CLK	RCC_APB2Periph_GPIOE
#define BF_DONE3_PIN		GPIO_Pin_15
#define	BF_DONE3_GPIO_PORT	GPIOE
#define BF_DONE3_GPIO_CLK	RCC_APB2Periph_GPIOE




/**
  * @brief  LM75 Temperature Sensor I2C Interface pins
  */  
#define LM75_I2C                         I2C1
#define LM75_I2C_CLK                     RCC_APB1Periph_I2C1
#define LM75_I2C_SCL_PIN                 GPIO_Pin_6                  /* PB.06 */
#define LM75_I2C_SCL_GPIO_PORT           GPIOB                       /* GPIOB */
#define LM75_I2C_SCL_GPIO_CLK            RCC_APB2Periph_GPIOB
#define LM75_I2C_SDA_PIN                 GPIO_Pin_7                  /* PB.07 */
#define LM75_I2C_SDA_GPIO_PORT           GPIOB                       /* GPIOB */
#define LM75_I2C_SDA_GPIO_CLK            RCC_APB2Periph_GPIOB
#define LM75_I2C_SMBUSALERT_PIN          GPIO_Pin_5                  /* PB.05 */
#define LM75_I2C_SMBUSALERT_GPIO_PORT    GPIOB                       /* GPIOB */
#define LM75_I2C_SMBUSALERT_GPIO_CLK     RCC_APB2Periph_GPIOB
#define LM75_I2C_DR                      ((uint32_t)0x40005410)
#define LM75_I2C1_EV_IRQn					I2C1_EV_IRQn
#define	LM75_I2C1_ER_IRQn					I2C1_ER_IRQn

#define LM75_DMA_CLK                     RCC_AHBPeriph_DMA1
#define LM75_DMA_TX_CHANNEL              DMA1_Channel6
#define LM75_DMA_RX_CHANNEL              DMA1_Channel7
#define LM75_DMA_TX_TCFLAG               DMA1_FLAG_TC6
#define LM75_DMA_RX_TCFLAG               DMA1_FLAG_TC7 

/**
  * @brief  ADC Interface pins
  */ 
#define	BF_ADC						ADC1
#define	BF_ADC_CLK					RCC_APB2Periph_ADC1
#define BF_ADC_CH0_PIN				GPIO_Pin_0
#define BF_ADC_CH1_PIN				GPIO_Pin_1
#define	BF_ADC_CH10_PIN				GPIO_Pin_0
#define	BF_ADC_CH11_PIN				GPIO_Pin_1
#define	BF_ADC_CH12_PIN				GPIO_Pin_2
#define	BF_ADC_CH13_PIN				GPIO_Pin_3
#define BF_ADC_GPIOA_PORT			GPIOA
#define	BF_ADC_GPIOC_PORT			GPIOC
#define	BF_ADC_CH0_GPIO_CLK			RCC_APB2Periph_GPIOC
#define BF_ADC_CH1_GPIO_CLK			RCC_APB2Periph_GPIOC
#define	BF_ADC_CH10_GPIO_CLK			RCC_APB2Periph_GPIOC
#define	BF_ADC_CH11_GPIO_CLK			RCC_APB2Periph_GPIOC
#define	BF_ADC_CH12_GPIO_CLK			RCC_APB2Periph_GPIOC
#define	BF_ADC_CH13_GPIO_CLK			RCC_APB2Periph_GPIOC
#define	BF_ADC_IRQn					ADC1_2_IRQn
#define	BF_ADC_REG_DR				ADC1_BASE + 0x4C

#define BF_ADC_DMA_CLK				RCC_AHBPeriph_DMA1
#define	BF_ADC_DMA_CHANNEL				DMA1_Channel1
#define	BF_ADC_DMA_TCFLAG				DMA1_FLAG_TC1
#define	BF_ADC_DMA_CHANNEL_IRQn			DMA1_Channel1_IRQn

#define BF_ADC_PWR_12V				0
#define BF_ADC_PWR_1V0_02			1
#define BF_ADC_PWR_1V0_13			2
#define	BF_ADC_PWR_3V3				3
#define	BF_TEMP_0					5		//PCB 1.0a, u7		,ADC ch1, 
#define	BF_TEMP_1					4		//PCB 1.0a, u6		,ADC ch0, PCB layout swapped


/**
  * @brief  BF power control Interface pins
  */
#define	BF_POWER_ENVCORE02_PIN			GPIO_Pin_12
#define	BF_POWER_ENVCORE02_GPIO_PORT	GPIOB
#define	BF_POWER_ENVCORE02_GPIO_CLK		RCC_APB2Periph_GPIOB
#define	BF_POWER_ENVCORE13_PIN			GPIO_Pin_11
#define	BF_POWER_ENVCORE13_GPIO_PORT	GPIOC
#define	BF_POWER_ENVCORE13_GPIO_CLK		RCC_APB2Periph_GPIOC
#define	BF_POWER_EN33BF_PIN				GPIO_Pin_12
#define BF_POWER_EN33BF_GPIO_PORT		GPIOC
#define BF_POWER_EN33BF_GPIO_CLK		RCC_APB2Periph_GPIOC

#define BF_POWER_EN33BF2_PIN			GPIO_Pin_8
#define BF_POWER_EN33BF2_GPIO_PORT		GPIOD
#define BF_POWER_EN33BF2_GPIO_CLK		RCC_APB2Periph_GPIOD

/**
  * @brief  BF timer control Interface pins
  */
#define BF_TIMER1_CH1N_PIN				GPIO_Pin_8
#define	BF_TIMER1_CH1_PIN				GPIO_Pin_9
#define	BF_TIMER1_GPIO_PORT				GPIOE
#define	BF_TIMER1_GPIO_CLK				RCC_APB2Periph_GPIOE
#define	BF_TIMER1_CLK					RCC_APB2Periph_TIM1

#define BF_TIMER2_CLK					RCC_APB1Periph_TIM2
#define BF_TIMER2_IRQn					TIM2_IRQn

#define BF_TIMER3_CLK					RCC_APB1Periph_TIM3
#define BF_TIMER3_IRQn					TIM3_IRQn

/* 
	BF GPIO pins
*/
#define BF_SEL_SATAn_PCIE				GPIO_Pin_4
#define BF_SEL_SATAn_PCIE_PORT			GPIOD
#define BF_SEL_SATAn_PCIE_PORT_CLK		RCC_APB2Periph_GPIOD

/*  
	BF FAN Control Pins
*/
#define BF_FAN_PWR_ON					GPIO_Pin_9
#define BF_FAN_PWR_ON_PORT				GPIOB
#define BF_FAN_PWR_ON_PORT_CLK			RCC_APB2Periph_GPIOB
#define BF_FAN_PWM						GPIO_Pin_8
#define BF_FAN_PWM_PORT					GPIOB
#define BF_FAN_PWM_PORT_CLK				RCC_APB2Periph_GPIOB
#define BF_FAN_SPEED					GPIO_Pin_8
#define BF_FAN_SPEEN_PORT				GPIOA
#define BF_FAN_SPEED_PORT_CLK			RCC_APB2Periph_GPIOA




void Set_System(void);
void Set_USBClock(void);
void BF_SystemInterruptsConfig(void);
void USB_Cable_Config (FunctionalState NewState);

void LM75_LowLevel_Init(void);
//void USART_Send_Data(COM_TypeDef COM, u8* data_buffer, u8 Nb_bytes);
void LM75_LowLevel_DeInit(void);


#endif /* __BF_SIMU_PLATFORM_H */

