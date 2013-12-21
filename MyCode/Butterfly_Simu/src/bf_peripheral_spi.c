/*
  ******************************************************************************
  * @file    bf_peripheral_spi.c
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   SPI routines prototypes
  ******************************************************************************
*/
#include "bf_general.h"

#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
//#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_endp.h"
#include "usb_core.h"
#include "stm32f10x.h"
#include "asic_engine.h"




#include "bf_peripheral_uart.h"
#include "bf_simu_platform.h"

#include "bf_peripheral_adc.h"
#include "bf_peripheral_spi.h"
#include "stm32f10x_spi.h"
#include "bf_simu_peripheral.h"

u32 dwSpiFlashID = 0;
u16 wSpiReadTemp1 = 1;
u16 wSpiReadTemp2 = 1;

void SPI_LowLevel_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;


  /*!< SPI_CS_GPIO, SPI_MOSI_GPIO, SPI_MISO_GPIO 
       and SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(BF_SPI_CS_GPIO_CLK | BF_SPI_MOSI_GPIO_CLK | BF_SPI_MISO_GPIO_CLK |
                         BF_SPI_SCK_GPIO_CLK, ENABLE);

  /*!< SPI Periph clock enable */
  RCC_APB2PeriphClockCmd(BF_SPI_CLK, ENABLE);

  /*!< Configure SPI_CS_PIN pin:  CS pin */
  GPIO_InitStructure.GPIO_Pin = BF_SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(BF_SPI_CS_GPIO_PORT, &GPIO_InitStructure);
  /*!< Deselect the BF: Chip Select high */
  SPI_CS_HIGH();

  
  /*!< Configure SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = BF_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(BF_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = BF_SPI_MOSI_PIN;
  GPIO_Init(BF_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = BF_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_Init(BF_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  

}

void SPI_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Disable the BF_SPI  */
  SPI_Cmd(BF_SPI, DISABLE);
  
  /*!< DeInitializes the BF_SPI */
  SPI_I2S_DeInit(BF_SPI);
  
  /*!< SPI Periph clock disable */
  RCC_APB2PeriphClockCmd(BF_SPI_CLK, DISABLE);
  
  /*!< Configure SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = BF_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(BF_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = BF_SPI_MISO_PIN;
  GPIO_Init(BF_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = BF_SPI_MOSI_PIN;
  GPIO_Init(BF_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SPI_CS_PIN pin: sFLASH Card CS pin */
  GPIO_InitStructure.GPIO_Pin = BF_SPI_CS_PIN;
  GPIO_Init(BF_SPI_CS_GPIO_PORT, &GPIO_InitStructure);
}

void BF_SPI_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;

  SPI_LowLevel_Init();

  ASIC_LowLevel_Init();
    
  /*!< Deselect the BF: Chip Select high */
  SPI_CS_HIGH();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(BF_SPI, &SPI_InitStructure);

  /*!< Enable the SPI  */
  SPI_Cmd(BF_SPI, ENABLE);
}

uint32_t BF_SpiSimu_FlashReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /*!< Select the FLASH: Chip Select low */
  SPI_CS_LOW();

  /*!< Send "RDID " instruction */
  SPI_SendWord(0x9F);

  /*!< Read a byte from the FLASH */
  Temp0 = SPI_SendWord(BF_SPI_DUMMY_WORD);

  /*!< Read a byte from the FLASH */
  Temp1 = SPI_SendWord(BF_SPI_DUMMY_WORD);

  /*!< Read a byte from the FLASH */
  Temp2 = SPI_SendWord(BF_SPI_DUMMY_WORD);

  /*!< Deselect the FLASH: Chip Select high */
  SPI_CS_HIGH();

  dwSpiFlashID = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
u8 SPI_SendByte(u8 cSendByte)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(BF_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(BF_SPI, cSendByte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(BF_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(BF_SPI);
}

u16 SPI_SendWord(u16 wSendWord)
{
	/*!< Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(BF_SPI, SPI_I2S_FLAG_TXE) == RESET);

	/*!< Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(BF_SPI, wSendWord);

	/*!< Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(BF_SPI, SPI_I2S_FLAG_RXNE) == RESET);

	/*!< Return the byte read from the SPI bus */
	wSpiReadTemp2 = SPI_I2S_ReceiveData(BF_SPI);
	return wSpiReadTemp2;
}

u16 SPI_ReadWord(void)
{
	/*!< Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(BF_SPI, SPI_I2S_FLAG_TXE) == RESET);

	/*!< Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(BF_SPI, 0xFFFF);

	/*!< Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(BF_SPI, SPI_I2S_FLAG_RXNE) == RESET);

	/*!< Return the byte read from the SPI bus */
	wSpiReadTemp1 = SPI_I2S_ReceiveData(BF_SPI);
	return wSpiReadTemp1;
}

