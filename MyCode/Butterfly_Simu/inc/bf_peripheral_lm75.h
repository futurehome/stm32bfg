/*
  ******************************************************************************
  * @file    bf_peripheral_lm75l.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    04-Oct-2013
  * @brief   LM75 routines prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BF_PERIPHERAL_LM75_H
#define __BF_PERIPHERAL_LM75_H

/**
  * @brief  Block Size
  */
#define LM75_REG_TEMP       0x00  /*!< Temperature Register of LM75 */
#define LM75_REG_CONF       0x01  /*!< Configuration Register of LM75 */
#define LM75_REG_THYS       0x02  /*!< Temperature Register of LM75 */
#define LM75_REG_TOS        0x03  /*!< Over-temp Shutdown threshold Register of LM75 */
#define I2C_TIMEOUT         ((uint32_t)0x3FFFF) /*!< I2C Time out */
#define LM75_ADDR           0x90   /*!< LM75 address */
#define LM75_I2C_SPEED      100000 /*!< I2C Speed */
#define LM75_WRITE_ADDR           0x90
#define LM75_READ_ADDR           0x91



/*I2C flow control keyword*/
#define I2CFLOW_WRITE_DATA	'D'
#define I2CFLOW_READ_DATA	'd'
#define I2CFLOW_START		'S'
#define I2CFLOW_RESTART		's'
#define I2CFLOW_SET_ACK		'A'
#define	I2CFLOW_RESET_ACK	'a'
#define	I2CFLOW_NACK_AND_STOP	'p'
#define	I2CFLOW_STOP		'P'

#define I2C1_EV_ISR I2C1_EV_IRQHandler

#define I2C_REG_CR1		0x00
#define I2C_REG_CR2		0x04
#define I2C_REG_OAR1	0x08
#define I2C_REG_OAR2	0x0C
#define I2C_REG_DR		0x10
#define	I2C_REG_SR1		0x14
#define	I2C_REG_SR2		0x18
#define	I2C_REG_CCR		0x1C
#define	I2C_REG_TRISE	0x20

#define CR2_ITBUFEN_SET	0x0400
#define	CR2_ITEVTEN_SET	0x0200

/* I2C SPE mask */
#define CR1_PE_Set              ((uint16_t)0x0001)
#define CR1_PE_Reset            ((uint16_t)0xFFFE)

/* I2C START mask */
#define CR1_START_Set           ((uint16_t)0x0100)
#define CR1_START_Reset         ((uint16_t)0xFEFF)

/* I2C STOP mask */
#define CR1_STOP_Set            ((uint16_t)0x0200)
#define CR1_STOP_Reset          ((uint16_t)0xFDFF)

/* I2C ACK mask */
#define CR1_ACK_Set             ((uint16_t)0x0400)
#define CR1_ACK_Reset           ((uint16_t)0xFBFF)

/* I2C ENGC mask */
#define CR1_ENGC_Set            ((uint16_t)0x0040)
#define CR1_ENGC_Reset          ((uint16_t)0xFFBF)

/* I2C SWRST mask */
#define CR1_SWRST_Set           ((uint16_t)0x8000)
#define CR1_SWRST_Reset         ((uint16_t)0x7FFF)

/* I2C PEC mask */
#define CR1_PEC_Set             ((uint16_t)0x1000)
#define CR1_PEC_Reset           ((uint16_t)0xEFFF)

/* I2C ENPEC mask */
#define CR1_ENPEC_Set           ((uint16_t)0x0020)
#define CR1_ENPEC_Reset         ((uint16_t)0xFFDF)

/* I2C ENARP mask */
#define CR1_ENARP_Set           ((uint16_t)0x0010)
#define CR1_ENARP_Reset         ((uint16_t)0xFFEF)

/* I2C NOSTRETCH mask */
#define CR1_NOSTRETCH_Set       ((uint16_t)0x0080)
#define CR1_NOSTRETCH_Reset     ((uint16_t)0xFF7F)

/* I2C registers Masks */
#define CR1_CLEAR_Mask          ((uint16_t)0xFBF5)

/* I2C DMAEN mask */
#define CR2_DMAEN_Set           ((uint16_t)0x0800)
#define CR2_DMAEN_Reset         ((uint16_t)0xF7FF)

/* I2C LAST mask */
#define CR2_LAST_Set            ((uint16_t)0x1000)
#define CR2_LAST_Reset          ((uint16_t)0xEFFF)

/* I2C FREQ mask */
#define CR2_FREQ_Reset          ((uint16_t)0xFFC0)

/* I2C ADD0 mask */
#define OAR1_ADD0_Set           ((uint16_t)0x0001)
#define OAR1_ADD0_Reset         ((uint16_t)0xFFFE)

/* I2C ENDUAL mask */
#define OAR2_ENDUAL_Set         ((uint16_t)0x0001)
#define OAR2_ENDUAL_Reset       ((uint16_t)0xFFFE)

/* I2C ADD2 mask */
#define OAR2_ADD2_Reset         ((uint16_t)0xFF01)

/* I2C F/S mask */
#define CCR_FS_Set              ((uint16_t)0x8000)

/* I2C CCR mask */
#define CCR_CCR_Set             ((uint16_t)0x0FFF)

/* I2C FLAG mask */
#define FLAG_Mask               ((uint32_t)0x00FFFFFF)

/* I2C Interrupt Enable mask */
#define ITEN_Mask               ((uint32_t)0x07000000)



//#define	I2C_REG_CR1_SWRST	

void LM75_DeInit(void);
void LM75_Init(void);
ErrorStatus LM75_GetStatus(void);
uint16_t LM75_ReadTemp(void);
uint16_t LM75_ReadReg(uint8_t RegName);
uint8_t LM75_WriteReg(uint8_t RegName, uint16_t RegValue);
uint8_t LM75_ReadConfReg(void);
uint8_t LM75_WriteConfReg(uint8_t RegValue);
uint8_t LM75_ShutDown(FunctionalState NewState);
static void LM75_DMA_Config(LM75_DMADirection_TypeDef Direction, uint8_t* buffer, uint8_t NumData);
u16 GetLm75Temperature(void);
void I2C1_EV_ISR(void);
u8 StartLm75(void);





#endif /* __BF_PERIPHERAL_LM75_H */


