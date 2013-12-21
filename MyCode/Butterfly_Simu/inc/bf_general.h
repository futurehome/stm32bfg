/*
  ******************************************************************************
  * @file    bf_general.h
  * @author  Jiang Jun
  * @version V1.0.0
  * @date    04-Oct-2013
  * @brief   BF general routines prototypes
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BF_GENERAL_H
#define __BF_GENERAL_H


#include "bf_peripheral_uart.h"


typedef enum _USB_SEND_TYPE
{
	cSOF = 0,
	cDATA = 1,
	cComLoop = 2
}USB_SEND_TYPE;

typedef struct
{
	u8 cEngineCount;
	u8 cFrequencySlect;
	u32 dwCalcTimePerChip;		//unit is mS, when we will get nonce result, use it to set softtimer 
}_BF_CHIP_STATUS;

typedef enum
{
	IDLE = 0,
	MINING,
	DONE_HAVE_NONCE,		// job done, but the result have not been fetched
	DONE_NO_NONCE
}__CHIP_WORKING_STATE;

#define SHA_MIDSTATE_SIZE	32
#define SHA_BLOCKDATA_SIZE	12
#define MAX_NONCE_IN_RESULT	8


typedef struct
{
	__CHIP_WORKING_STATE  ChipState;
	u8  cMidstate[SHA_MIDSTATE_SIZE];
	u8  cBlockdata[SHA_BLOCKDATA_SIZE];
	u8	cEngineDoneCount;
	u8	cNonceCount;
	u32	dwNonceList[MAX_NONCE_IN_RESULT];
	u8 	cEngineCount;
	u8 	cFrequencySlect;
	u32 dwCalcTimePerChip;		//unit is mS, when we will get nonce result, use it to set softtimer 
}__CHIP_PROCESSING_STATUS;


#define BF_SUCCESS 0
#define BF_BUFF_SMALL 1
#define BF_FAIL 2

#define BF_TEMPERATURE_COMPENSATE 5

u16 UsbWrite(u8* cpSourceBuffer, u16 wSendLength);
u8 MemoryCopy(u8 * cpSourceBuffer, u8 * cpTargetBuffer, u16 wLength);
void HostCommandDataProcess(void);
void HostCommandErrorProcess(u16 wErrorCode);
void UsbSendBufferProcess(u8 ucEndpointNumber);
u8 UsbLoopSend(u16 wLength);
void UartCom2RxProcess(void);
u16 UsbToComTransfer(COM_TypeDef cComPort, u16 wTransferLength);
u16 ComToUsbTransfer(COM_TypeDef cComPort, u16 wTransferLength);
void ClearUsbRxBuffer(void);

u16 	__ARM_UsbSendData(u8* cpSourceBuffer, u16 wSendLength);
u8 		__ARM_USB_GetInformation(void);
u16 	__ARM_UsbGetData(u8* cpTargetBuffer, u16 wExpectLength);
void 	__ARM_SC_Initialize(void);
void 	__ARM_ASIC_Activate_CS(void);
void 	__ARM_ASIC_Deactivate_CS(void);
u16 	__ARM_SC_WriteData(u8 iChip, u16 iEngine, u8 iAdrs, u16 iData);
u16 	__ARM_SC_ReadData (u8 iChip, u16 iEngine, u8 iAdrs);

void BF_CommandPrepare(void);
int __ARM_A2D_GetTemp1(void);
int __ARM_A2D_GetTemp2(void);
void __ARM_FAN_SetSpeed(u8 iSpeed);
int __ARM_A2D_Get3P3V(void);
int __ARM_A2D_Get1V_02(void);
int __ARM_A2D_Get1V_13(void);

int __ARM_A2D_Get12V(void);
void GlobalVariableInitialize(void);
void UartBackDoor(u32 dwData);
void UartBackDoorByte(u8 cData);
void UartBackDoorWord(u16 wData);
void GetBFCalculateTime(u8 cChip);


















#endif /* __BF_GENERAL_H */

