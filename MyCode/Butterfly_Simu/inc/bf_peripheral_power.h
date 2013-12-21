/*
  ******************************************************************************
  * @file    bf_peripheral_power.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   power control routines prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BF_PERIPHERAL_POWER_H
#define __BF_PERIPHERAL_POWER_H

#include "stm32f10x.h"

#define POWER_ON	1
#define POWER_OFF	0

#define	PWR_ENVCORE02	1
#define	PWR_ENVCORE13	2
#define	PWR_EN33BF		3

void BF_PowerControlLowLevelInit(void);

void BF_PowerControl(u8 cPowerControlPin, u8 cPowerControl);
void BF_PowerInit(void);
void BF_GpioInit(void);




#endif /* __BF_PERIPHERAL_POWER_H */

