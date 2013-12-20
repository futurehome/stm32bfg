/*
  ******************************************************************************
  * @file    bf_peripheral_power.c
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   power control routines prototypes
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
#include "bf_peripheral_power.h"


/**
  * @brief  Initializes the power control pins.
  * @param  None
  * @retval None
  */
void BF_PowerControlLowLevelInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /*!< Power control Periph clock enable */
  RCC_APB2PeriphClockCmd(BF_POWER_ENVCORE02_GPIO_CLK | BF_POWER_ENVCORE13_GPIO_CLK
  							| BF_POWER_EN33BF_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(BF_POWER_EN33BF2_GPIO_CLK, ENABLE);
    
  /*!< Configure EN_VCORE02 pin  */
  GPIO_InitStructure.GPIO_Pin = BF_POWER_ENVCORE02_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(BF_POWER_ENVCORE02_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure EN_VCORE13 pin */
  GPIO_InitStructure.GPIO_Pin = BF_POWER_ENVCORE13_PIN;
  GPIO_Init(BF_POWER_ENVCORE13_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure EN_33BF pin */
  GPIOC->ODR |= 0x00001000;	 //1.0V control , 0= off
  GPIO_InitStructure.GPIO_Pin = BF_POWER_EN33BF_PIN;
  GPIO_Init(BF_POWER_EN33BF_GPIO_PORT, &GPIO_InitStructure); 

	GPIO_ResetBits(GPIOD, 0x0100);		//new BF_3.3 control, power off


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		//new BF 3.3 power control pin
	GPIO_Init(GPIOD , &GPIO_InitStructure); 

}


void BF_PowerControl(u8 cPowerControlPin, u8 cPowerControl)
{
	GPIO_TypeDef* GPIO_Port;
	
	if(cPowerControlPin == PWR_ENVCORE02)
	{
		GPIO_Port = BF_POWER_ENVCORE02_GPIO_PORT;
		if(cPowerControl == POWER_ON)
		{
			GPIO_Port->ODR |= BF_POWER_ENVCORE02_PIN;
		}
		else
		{
			GPIO_Port->ODR &= (~BF_POWER_ENVCORE02_PIN);
		}
	}
	else if(cPowerControlPin == PWR_ENVCORE13)
	{
		GPIO_Port = BF_POWER_ENVCORE13_GPIO_PORT;
		if(cPowerControl == POWER_ON)
		{
			GPIO_Port->ODR |= BF_POWER_ENVCORE13_PIN;
		}
		else
		{
			GPIO_Port->ODR &= (~BF_POWER_ENVCORE13_PIN);
		}
	}
	else if(cPowerControlPin == PWR_EN33BF)
	{
		GPIO_Port = BF_POWER_EN33BF_GPIO_PORT;
		if(cPowerControl == POWER_ON)
		{
			GPIO_Port->ODR |= BF_POWER_EN33BF_PIN;
		}
		else
		{
			GPIO_Port->ODR &= (~BF_POWER_EN33BF_PIN);
		}
	}
}

void BF_PowerInit(void)
{
	u32 dwDelay;
	GPIO_TypeDef * GPIO_PortD;
	
	GPIO_PortD = GPIOD;
	
	//GPIO_PortD->ODR |= 0xFFFFFEFF;	 //clear UART3 TXD, use it for BF_3.3 power control, 

	BF_PowerControl(PWR_ENVCORE02, POWER_OFF);
	BF_PowerControl(PWR_ENVCORE13, POWER_OFF);
	BF_PowerControl(PWR_EN33BF, POWER_OFF);
	

	BF_PowerControlLowLevelInit();

	BF_PowerControl(PWR_ENVCORE02, POWER_ON);
	BF_PowerControl(PWR_ENVCORE13, POWER_ON);
	BF_PowerControl(PWR_EN33BF, POWER_OFF);
		dwDelay = 0x20ff;						//delay at 90% 1.0 slope, fail(0x40ff)
												//delay at 50% 1.0 slope , spi ok (0x30FF)
												//tested at 20% 1.0 slope, 2nd write feedback ok(0x20FF)
												//tested before 0.5mS, spi fail(0x10FF)
												//tested 0x18FF, fail, before some uS
												//tested at 70% slope, spi ok(0x38FF)
												//tested 0x28FF, failed 2times
			while((dwDelay--) != 0)
			{
			}
	//GPIO_SetBits(GPIOD, 0x0100);		//new BF_3.3 control, power on
	GPIO_PortD->ODR |= 0x00000100;	 // UART3 TXD, use it for BF_3.3 power control, on
}

void BF_GpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_TypeDef * GPIO_Port;

	
  /*Configure using SATA or PCIE for xlink*/
	
  /*!< Select SATA or PCIe for UART communication, set Periph clock enable */
	RCC_APB2PeriphClockCmd(BF_SEL_SATAn_PCIE_PORT_CLK, ENABLE);

 	/*!< Configure SATA/PCIE select pins: SEL_SATA#/PCIE */
	GPIO_Port = BF_SEL_SATAn_PCIE_PORT;

	GPIO_InitStructure.GPIO_Pin = BF_SEL_SATAn_PCIE;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(BF_SEL_SATAn_PCIE_PORT, &GPIO_InitStructure);

	GPIO_Port->ODR &= (~BF_SEL_SATAn_PCIE);	 // force  = 0 to select SATA port
	//GPIO_Port->ODR |= (BF_SEL_SATAn_PCIE);	 // force  = 1 to select pcie port
  
}

