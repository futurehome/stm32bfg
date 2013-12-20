/*
  ******************************************************************************
  * @file    bf_peripheral_spi.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   SPI routines prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BF_PERIPHERAL_SPI_H
#define __BF_PERIPHERAL_SPI_H

#include "stm32f10x.h"

/*
  * @brief  : SPI Chip Select pin low
  */
#define SPI_CS_LOW()       GPIO_ResetBits(BF_SPI_CS_GPIO_PORT, BF_SPI_CS_PIN)
/**
  * @brief  Deselect : SPI Chip Select pin high
  */
#define SPI_CS_HIGH()      GPIO_SetBits(BF_SPI_CS_GPIO_PORT, BF_SPI_CS_PIN) 

#define BF_SPI_DUMMY_WORD	0xFFFF



void SPI_LowLevel_Init(void);
void SPI_LowLevel_DeInit(void);
void BF_SPI_Init(void);
u32 BF_SpiSimu_FlashReadID(void);
u8 SPI_SendByte(u8 cSendByte);
u16 SPI_SendWord(u16 wSendWord);
u16 SPI_ReadWord(void);








#endif /* __BF_PERIPHERAL_SPI_H */


