/*
  ******************************************************************************
  * @file    bf_simu_peripheral.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   Uusb endpoint routines prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BF_SIMU_PERIPHERAL_H
#define __BF_SIMU_PERIPHERAL_H

 /** 
  * @brief  IOE DMA Direction  
  */ 
typedef enum
{
	LM75_DMA_TX = 0,
	LM75_DMA_RX = 1
}LM75_DMADirection_TypeDef;

/** 
  * @brief  TSENSOR Status  
  */ 
typedef enum
{
	LM75_OK = 0,
	LM75_FAIL
}LM75_Status_TypDef;

/* Uncomment the following line to use Timeout_User_Callback LM75_TimeoutUserCallback(). 
   If This Callback is enabled, it should be implemented by user in main function .
   LM75_TimeoutUserCallback() function is called whenever a timeout condition 
   occure during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...). */   
/* #define USE_TIMEOUT_USER_CALLBACK */    
    
/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will 
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define LM75_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define LM75_LONG_TIMEOUT         ((uint32_t)(10 * LM75_FLAG_TIMEOUT))    
    

/**
  * @brief  Block Size
  */
#define LM75_REG_TEMP       0x00  /*!< Temperature Register of LM75 */
#define LM75_REG_CONF       0x01  /*!< Configuration Register of LM75 */
#define LM75_REG_THYS       0x02  /*!< Temperature Register of LM75 */
#define LM75_REG_TOS        0x03  /*!< Over-temp Shutdown threshold Register of LM75 */
#define I2C_TIMEOUT         ((uint32_t)0x3FFFF) /*!< I2C Time out */
#define LM75_WRITE_ADDR     0x90   /*!< LM75 write address */
#define LM75_READ_ADDR		0x91	/* LM75 read address*/
#define LM75_ADDR			0x90	/* LM75 address*/
#define LM75_I2C_SPEED      100000 /*!< I2C Speed */

/** 
  * @brief  Timeout user callback function. This function is called when a timeout
  *         condition occurs during communication with IO Expander. Only protoype
  *         of this function is decalred in IO Expander driver. Its implementation
  *         may be done into user application. This function may typically stop
  *         current operations and reset the I2C peripheral and IO Expander.
  *         To enable this function use uncomment the define USE_TIMEOUT_USER_CALLBACK
  *         at the top of this file.          
  */
#ifdef USE_TIMEOUT_USER_CALLBACK 
 uint8_t LM75_TIMEOUT_UserCallback(void);
#else
 #define LM75_TIMEOUT_UserCallback()  LM75_FAIL
#endif /* USE_TIMEOUT_USER_CALLBACK */

/**
  * @brief  M25P SPI Flash supported commands
  */  
#define sFLASH_CMD_WRITE          0x02  /*!< Write to Memory instruction */
#define sFLASH_CMD_WRSR           0x01  /*!< Write Status Register instruction */
#define sFLASH_CMD_WREN           0x06  /*!< Write enable instruction */
#define sFLASH_CMD_READ           0x03  /*!< Read from Memory instruction */
#define sFLASH_CMD_RDSR           0x05  /*!< Read Status Register instruction  */
#define sFLASH_CMD_RDID           0x9F  /*!< Read identification */
#define sFLASH_CMD_SE             0xD8  /*!< Sector Erase instruction */
#define sFLASH_CMD_BE             0xC7  /*!< Bulk Erase instruction */

#define sFLASH_WIP_FLAG           0x01  /*!< Write In Progress (WIP) flag */

#define sFLASH_DUMMY_BYTE         0xA5
#define sFLASH_SPI_PAGESIZE       0x100

#define sFLASH_M25P128_ID         0x202018
#define sFLASH_M25P64_ID          0x202017





void sFLASH_DeInit(void);
void sFLASH_Init(void);
void sFLASH_EraseSector(uint32_t SectorAddr);
void sFLASH_EraseBulk(void);
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t sFLASH_ReadID(void);
void sFLASH_StartReadSequence(uint32_t ReadAddr);

/**
  * @brief  Low layer functions
  */
uint8_t sFLASH_ReadByte(void);
uint8_t sFLASH_SendByte(uint8_t byte);
uint16_t sFLASH_SendHalfWord(uint16_t HalfWord);
void sFLASH_WriteEnable(void);
void sFLASH_WaitForWriteEnd(void);


#endif /* __BF_SIMU_PERIPHERAL_H */
