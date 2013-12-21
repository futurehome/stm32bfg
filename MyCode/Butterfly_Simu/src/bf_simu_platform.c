/*
  ******************************************************************************
  * @file    bf_simu_platform.c
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    15-Sep-2013
  * @brief   Usb end point routines prototypes
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
//#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_endp.h"
#include "usb_core.h"
#include "bf_general.h"

#include "bf_peripheral_uart.h"
#include "bf_simu_platform.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;




uint8_t  USB_Tx_State = 0;

u8 cIdleData[0x2];
/* Extern variables ----------------------------------------------------------*/

extern USART_TypeDef* COM_USART[COMn]; 

extern GPIO_TypeDef* COM_TX_PORT[COMn];
 
extern GPIO_TypeDef* COM_RX_PORT[COMn];
 
extern const uint32_t COM_USART_CLK[COMn];

extern const uint32_t COM_TX_PORT_CLK[COMn];
 
extern const uint32_t COM_RX_PORT_CLK[COMn];

extern const uint16_t COM_TX_PIN[COMn];

extern const uint16_t COM_RX_PIN[COMn];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	/*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
	*/   

	/* Enable USB_DISCONNECT GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);

	/* Configure USB pull-up pin */
	GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);

	/* Configure the EXTI line 18 connected internally to the USB IP */
	EXTI_ClearITPendingBit(EXTI_Line18);
	EXTI_InitStructure.EXTI_Line = EXTI_Line18; 
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}


/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz)
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
	/* Select USBCLK source */
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  
	/* Enable the USB clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}


/*******************************************************************************
* Function Name  : USB_Interrupts_Config
* Description    : Configures the USB interrupts
* Input          : None.
* Return         : None.
*******************************************************************************/
void BF_SystemInterruptsConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
  
	/* 2 bit for pre-emption priority, 2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
  
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
	/* Enable the USB Wake-up interrupt */					//usb wakeup disabled, Jiangjun
	//NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//NVIC_Init(&NVIC_InitStructure);


	/* Enable USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = BF_COM1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = BF_COM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable I2C1 Interrupt*/
	NVIC_InitStructure.NVIC_IRQChannel = LM75_I2C1_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	/*Enable DMA1 Channel1 Interrupt*/
	NVIC_InitStructure.NVIC_IRQChannel = BF_ADC_DMA_CHANNEL_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_Init(&NVIC_InitStructure);

	/*Enable Timer2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = BF_TIMER2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_Init(&NVIC_InitStructure);

	/*Enable Timer3 Interrupt */
	
	NVIC_InitStructure.NVIC_IRQChannel = BF_TIMER3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_Init(&NVIC_InitStructure);
	
}


/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable
* Input          : None.
* Return         : Status
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{
	if (NewState == DISABLE)
	{
		GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
	}
	else
	{
		GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
	}
}



/**
  * @brief  Initializes the LM75_I2C.
  * @param  None
  * @retval None
  */
void LM75_LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /*!< LM75_I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(LM75_I2C_CLK, ENABLE);
    
  /*!< LM75_I2C_SCL_GPIO_CLK, LM75_I2C_SDA_GPIO_CLK 
       and LM75_I2C_SMBUSALERT_GPIO_CLK Periph clock enable */
  RCC_APB2PeriphClockCmd(LM75_I2C_SCL_GPIO_CLK | LM75_I2C_SDA_GPIO_CLK |
                         LM75_I2C_SMBUSALERT_GPIO_CLK, ENABLE);
  
  /*!< Configure LM75_I2C pins: SCL */
  GPIO_InitStructure.GPIO_Pin = LM75_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(LM75_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure LM75_I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = LM75_I2C_SDA_PIN;
  GPIO_Init(LM75_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure LM75_I2C pin: SMBUS ALERT */
  GPIO_InitStructure.GPIO_Pin = LM75_I2C_SMBUSALERT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(LM75_I2C_SMBUSALERT_GPIO_PORT, &GPIO_InitStructure); 
}

/*
void USART_Send_Data(COM_TypeDef COM, u8* data_buffer, u8 Nb_bytes)
{
  
	uint32_t i;
  
	for (i = 0; i < Nb_bytes; i++)
	{
		USART_SendData(COM, *(data_buffer + i));
		while(USART_GetFlagStatus(COM, USART_FLAG_TXE) == RESET); 
	}  
}*/

/**
  * @brief  DeInitializes the LM75_I2C.
  * @param  None
  * @retval None
  */
void LM75_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /*!< Disable LM75_I2C */
  I2C_Cmd(LM75_I2C, DISABLE);
  /*!< DeInitializes the LM75_I2C */
  I2C_DeInit(LM75_I2C);
  
  /*!< LM75_I2C Periph clock disable */
  RCC_APB1PeriphClockCmd(LM75_I2C_CLK, DISABLE);
    
  /*!< Configure LM75_I2C pins: SCL */
  GPIO_InitStructure.GPIO_Pin = LM75_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(LM75_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure LM75_I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = LM75_I2C_SDA_PIN;
  GPIO_Init(LM75_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure LM75_I2C pin: SMBUS ALERT */
  GPIO_InitStructure.GPIO_Pin = LM75_I2C_SMBUSALERT_PIN;
  GPIO_Init(LM75_I2C_SMBUSALERT_GPIO_PORT, &GPIO_InitStructure);
}


