/*
 * AVR32_OptimizedTemplates.h
 *
 * Created: 19/01/2013 19:20:48
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */ 


#ifndef AVR32_OPTIMIZEDTEMPLATES_H_
#define AVR32_OPTIMIZEDTEMPLATES_H_


//////////////////////////////////////////////////////////////////
// Assembly optimized functions
//////////////////////////////////////////////////////////////////
void OPTIMIZED__AVR32_CPLD_Write(unsigned char iAdrs, unsigned char iData);
void OPTIMIZED__AVR32_CPLD_Read (unsigned char iAdrs, unsigned char *retVal);
void OPTIMIZED__AVR32_CPLD_BurstTxWrite(char* szData, char iAddress);
void OPTIMIZED__AVR32_CPLD_BurstRxRead(char* szData, char iAddress);

unsigned int OPTO_GetTickCountRet(void);

//////////////////////////////////////////////////////////////////
// MACROS
/////////////////////////////////////////////////////////////////
// Auto address increase function
#define CPLD_activate_address_increase     AVR32_GPIO.port[1].ovrs  = __AVR32_CPLD_INCREASE_ADDRESS;
#define CPLD_deactivate_address_increase   AVR32_GPIO.port[1].ovrc  = __AVR32_CPLD_INCREASE_ADDRESS;

//#if defined(__OPERATING_FREQUENCY_32MHz__)
//	#define MACRO_GetTickCount(x)  (x = (u32)((u32)(MAST_TICK_COUNTER) | (u32)(AVR32_TC.channel[0].cv)))
//	#define MACRO_GetTickCountRet  ((u32)((u32)(MAST_TICK_COUNTER) | (u32)(AVR32_TC.channel[0].cv)))
	//#define MACRO_GetTickCount(x)  (x = (u32)((u32)(MAST_TICK_COUNTER) | (u32)(AVR32_RTC.val * 9)))
	//#define MACRO_GetTickCountRet  ((u32)((u32)(MAST_TICK_COUNTER) | (u32)(AVR32_RTC.val * 9 )))	
//#else
//	#define MACRO_GetTickCount(x)  (x = (((u32)((u32)(MAST_TICK_COUNTER) | (u32)(AVR32_TC.channel[0].cv))) >> 1) )
//	#define MACRO_GetTickCountRet *(u32*)0xffffffff///MAST_TICK_COUNTER /////JJ//(((u32)((u32)(MAST_TICK_COUNTER) | (u32)(AVR32_TC.channel[0].cv))) >> 1)
	//#define MACRO_GetTickCount(x)  (x = (((u32)((u32)(MAST_TICK_COUNTER) | (u32)(AVR32_RTC.val))) >> 1) )
	//#define MACRO_GetTickCountRet  (((u32)((u32)(MAST_TICK_COUNTER) | (u32)(AVR32_RTC.val))) >> 1)	
//#endif


// Old Values 	AVR32_GPIO.port[1].ovr  = (AVR32_GPIO.port[1].ovr & 0x0FFFFFF00) | address; 

void MACRO_SET_OUTPUT(u32 value);
//{
/*
	AVR32_GPIO.port[1].ovrc  = 0x0FF; \
								  NOP_OPERATION \
								  AVR32_GPIO.port[1].ovrs  = value;
								  */
								  //}

void MACRO__AVR32_CPLD_Read(u32 ret_value, u32 address);
//{
/*
({ \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = __AVR32_CPLD_OE; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].oders = __AVR32_CPLD_BUS_ALL; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrs = __AVR32_CPLD_ADRS; \
		NOP_OPERATION \
		MACRO_SET_OUTPUT(address) \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrs = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrc = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = __AVR32_CPLD_ADRS; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].oderc = __AVR32_CPLD_BUS_ALL; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrs = __AVR32_CPLD_OE; \
		NOP_OPERATION \
		  int iRetVal = (AVR32_GPIO.port[1].pvr); \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = __AVR32_CPLD_OE; \
		AVR32_GPIO.port[1].oderc = __AVR32_CPLD_BUS_ALL; \
		ret_value = iRetVal; \
		NOP_OPERATION \
		})*/
//}

void MACRO__AVR32_CPLD_Write(u32 address, u32 value);
//{
/*({ \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = __AVR32_CPLD_OE; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].oders = __AVR32_CPLD_BUS_ALL; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrs = __AVR32_CPLD_ADRS; \
		NOP_OPERATION \
		MACRO_SET_OUTPUT(address) \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrs = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrc = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = __AVR32_CPLD_ADRS; \
		NOP_OPERATION \
		MACRO_SET_OUTPUT(value) \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrs = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrc = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].oderc = __AVR32_CPLD_BUS_ALL; \
		NOP_OPERATION })*/
//}
	

void MACRO__AVR32_CPLD_BurstTxWrite(char* szdata, u32 address_begin);
//{
/*
({ \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc =  __AVR32_CPLD_OE; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].oders = __AVR32_CPLD_BUS_ALL; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrs = __AVR32_CPLD_ADRS; \
		NOP_OPERATION \
		MACRO_SET_OUTPUT(address_begin) \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrs = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrc = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = __AVR32_CPLD_ADRS; \
		NOP_OPERATION \
		CPLD_activate_address_increase; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = 0x0FF; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrs = szdata[0]; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrs = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrc = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = 0x0FF; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrs = szdata[1]; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrs = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrc = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = 0x0FF; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrs = szdata[2]; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrs = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrc = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = 0x0FF; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrs = szdata[3]; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrs = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrc = __AVR32_CPLD_STROBE; \
		CPLD_deactivate_address_increase; \
		AVR32_GPIO.port[1].oderc = __AVR32_CPLD_BUS_ALL; \
		NOP_OPERATION }) */
	//	}
		
void MACRO__AVR32_CPLD_BurstRxRead(u32 iData, u32 iAddress);
//{
/*({ \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = __AVR32_CPLD_OE; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].oders = __AVR32_CPLD_BUS_ALL; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrs = __AVR32_CPLD_ADRS; \
		NOP_OPERATION \
		MACRO_SET_OUTPUT(iAddress) \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrs = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrc = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = __AVR32_CPLD_ADRS; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].oderc = __AVR32_CPLD_BUS_ALL; \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrs = __AVR32_CPLD_OE; \
		NOP_OPERATION \
		CPLD_activate_address_increase; \
		NOP_OPERATION \
		iData[0] = (AVR32_GPIO.port[1].pvr); \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrs = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrc = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		iData[1] = (AVR32_GPIO.port[1].pvr); \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrs = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrc = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		iData[2] = (AVR32_GPIO.port[1].pvr); \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrs = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		AVR32_GPIO.port[0].ovrc = __AVR32_CPLD_STROBE; \
		NOP_OPERATION \
		iData[3] = (AVR32_GPIO.port[1].pvr); \
		NOP_OPERATION \
		AVR32_GPIO.port[1].ovrc = __AVR32_CPLD_OE; \
		CPLD_deactivate_address_increase; \
		AVR32_GPIO.port[1].oderc = __AVR32_CPLD_BUS_ALL; \
		NOP_OPERATION \
	 })*/
//}




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#define __NO_ASSEMBLY_OPTIMIZATION__	1

#ifdef	__NO_ASSEMBLY_OPTIMIZATION__

#define MACRO_XLINK_clear_RX				   MACRO__AVR32_CPLD_Write(CPLD_ADDRESS_RX_CONTROL, CPLD_RX_CONTROL_CLEAR);
#define MACRO_XLINK_get_TX_status(ret_value)  (MACRO__AVR32_CPLD_Read(ret_value, CPLD_ADDRESS_TX_STATUS))
#define MACRO_XLINK_get_RX_status(ret_value)  (MACRO__AVR32_CPLD_Read(ret_value, CPLD_ADDRESS_RX_STATUS))
#define MACRO_XLINK_set_target_address(x)	  (MACRO__AVR32_CPLD_Write(CPLD_ADDRESS_TX_TARGET_ADRS, x & 0x1F))


void MACRO_XLINK_send_packet(char iadrs, char* szdata, char ilen, char lp, char bc);
//{
/*({ \
		  char read_tx_status = 0x0FF; \
		while ((read_tx_status & CPLD_TX_STATUS_TxInProg) != 0) { MACRO_XLINK_get_TX_status(read_tx_status);} \
		MACRO_XLINK_set_target_address(iadrs); \
		  unsigned char iTotalToSend = (ilen << 1); \
		  char szMMR[4]; \
		MACRO__AVR32_CPLD_BurstTxWrite(szdata, CPLD_ADDRESS_TX_BUF_BEGIN); \
		  char iTxControlVal = 0b00000000; \
		iTxControlVal |= iTotalToSend;	\
		iTxControlVal |= (lp != 0) ? CPLD_TX_CONTROL_LP : 0; \
		iTxControlVal |= (bc != 0) ? CPLD_TX_CONTROL_BC : 0; \
		MACRO__AVR32_CPLD_Write(CPLD_ADDRESS_TX_CONTROL, iTxControlVal); \
		MACRO__AVR32_CPLD_Write(CPLD_ADDRESS_TX_START, CPLD_ADDRESS_TX_START_SEND); \
		})	*/
//}



void MACRO_XLINK_wait_packet(char* data, unsigned int length, u32 time_out, char timeout_detected, char senders_address, char LP, char BC);
//{
/*({ \
		while(TRUE) \
		{ \
			BC = 0; \
			LP = 0; \
			timeout_detected = FALSE; \
			length = 0; \
			senders_address = 0; \
			  char iActualRXStatus = 0; \
			  unsigned char us1 = 0; \
			  unsigned char us2 = 0; \
			MACRO_XLINK_get_RX_status(iActualRXStatus); \
			  u32 iTimeoutHolder; \
			MACRO_GetTickCount(iTimeoutHolder); \
			  u32 iTickHolder; \
			if ((iActualRXStatus & CPLD_RX_STATUS_DATA) == 0) \
			{ \
				while (TRUE) \
				{ \
					MACRO_XLINK_get_RX_status(iActualRXStatus); \
					if ((iActualRXStatus & CPLD_RX_STATUS_DATA) != 0) break; \
					MACRO_GetTickCount(iTickHolder); \
					if ((u32)(iTickHolder - iTimeoutHolder) > (u32)time_out) \
					{ \
						timeout_detected = TRUE; \
						length = 0; \
						senders_address = 0; \
						break; \
					}  \
				} \
				if (timeout_detected == TRUE) break; \
			} \
			  char imrLen = 0; \
			imrLen = ((iActualRXStatus & 0b0111000) >> 3); \
			length = imrLen; \
			LP = ((iActualRXStatus & CPLD_RX_STATUS_LP) != 0) ? 1 : 0; \
			BC = ((iActualRXStatus & CPLD_RX_STATUS_BC) != 0) ? 1 : 0; \
			MACRO__AVR32_CPLD_Read(senders_address, CPLD_ADDRESS_SENDERS_ADRS); \
			MACRO__AVR32_CPLD_BurstRxRead(data, CPLD_ADDRESS_RX_BUF_BEGIN); \
			MACRO__AVR32_CPLD_Write(CPLD_ADDRESS_RX_CONTROL, CPLD_RX_CONTROL_CLEAR); \
			break; \
		} \
	}) */
	//}
	
	
void MACRO_XLINK_simulate_wait_packet(u32 data, u32 length, u32 time_out, u32 timeout_detected, u32 senders_address, u32 LP, u32 BC);
//{
/*({ \
while(TRUE) \
{ \
	BC = 0; \
	LP = 0; \
	timeout_detected = FALSE; \
	length = 0; \
	senders_address = 0; \
	  char iActualRXStatus = 0; \
	  unsigned char us1 = 0; \
	  unsigned char us2 = 0; \
	MACRO_XLINK_get_RX_status(iActualRXStatus); \
	u32 iTimeoutHolder; \
	MACRO_GetTickCount(iTimeoutHolder); \
	u32 iTickHolder; \
	  char imrLen = 0; \
	imrLen = ((iActualRXStatus & 0b0111000) >> 3); \
	length = imrLen; \
	LP = ((iActualRXStatus & CPLD_RX_STATUS_LP) != 0) ? 1 : 0; \
	BC = ((iActualRXStatus & CPLD_RX_STATUS_BC) != 0) ? 1 : 0; \
	MACRO__AVR32_CPLD_Read(senders_address, CPLD_ADDRESS_SENDERS_ADRS); \
	MACRO__AVR32_CPLD_BurstRxRead(data, CPLD_ADDRESS_RX_BUF_BEGIN); \
	MACRO__AVR32_CPLD_Write(CPLD_ADDRESS_RX_CONTROL, CPLD_RX_CONTROL_CLEAR); \
	break; \
} \
}) 
*/
//}


#else

// Nothing really...
#define MACRO_XLINK_clear_RX				   OPTIMIZED__AVR32_CPLD_Write(CPLD_ADDRESS_RX_CONTROL, CPLD_RX_CONTROL_CLEAR);
#define MACRO_XLINK_get_TX_status(ret_value)  (OPTIMIZED__AVR32_CPLD_Read(CPLD_ADDRESS_TX_STATUS, &ret_value))
#define MACRO_XLINK_get_RX_status(ret_value)  (OPTIMIZED__AVR32_CPLD_Read(CPLD_ADDRESS_RX_STATUS, &ret_value))
#define MACRO_XLINK_set_target_address(x)	  (OPTIMIZED__AVR32_CPLD_Write(CPLD_ADDRESS_TX_TARGET_ADRS, x & 0x1F))
	

void MACRO_XLINK_send_packet(char iadrs, char* szdata, char ilen, char lp, char bc)
{
/*({ \
							char read_tx_status = 0x0FF; \
							while ((read_tx_status & CPLD_TX_STATUS_TxInProg) != 0) { MACRO_XLINK_get_TX_status(read_tx_status);} \
							MACRO_XLINK_set_target_address(iadrs); \
							unsigned char iTotalToSend = (ilen << 1); \
							char szMMR[4]; \
							OPTIMIZED__AVR32_CPLD_BurstTxWrite(szdata, CPLD_ADDRESS_TX_BUF_BEGIN); \
							char iTxControlVal = 0b00000000; \
							iTxControlVal |= iTotalToSend;	\
							iTxControlVal |= (lp != 0) ? CPLD_TX_CONTROL_LP : 0; \
							iTxControlVal |= (bc != 0) ? CPLD_TX_CONTROL_BC : 0; \
							OPTIMIZED__AVR32_CPLD_Write(CPLD_ADDRESS_TX_CONTROL, iTxControlVal); \
							OPTIMIZED__AVR32_CPLD_Write(CPLD_ADDRESS_TX_START, CPLD_ADDRESS_TX_START_SEND); \
						})*/
}
						
/*
#define MACRO_XLINK_wait_packet(data, length, time_out, timeout_detected, senders_address, LP, BC) ({ \
while(TRUE) \
{ \
	BC = 0; \
	LP = 0; \
	timeout_detected = FALSE; \
	length = 0; \
	senders_address = 0; \
	  char iActualRXStatus = 0; \
	  unsigned char us1 = 0; \
	  unsigned char us2 = 0; \
	MACRO_XLINK_get_RX_status(iActualRXStatus); \
	u32 iTimeoutHolder; \
	MACRO_GetTickCount(iTimeoutHolder); \
	u32 iTickHolder; \
	if ((iActualRXStatus & CPLD_RX_STATUS_DATA) == 0) \
	{ \
		while (TRUE) \
		{ \
			MACRO_XLINK_get_RX_status(iActualRXStatus); \
			if ((iActualRXStatus & CPLD_RX_STATUS_DATA) != 0) break; \
			MACRO_GetTickCount(iTickHolder); \
			if ((iTickHolder - iTimeoutHolder) > time_out) \
			{ \
				timeout_detected = TRUE; \
				length = 0; \
				senders_address = 0; \
				break; \
			}  \
		} \
		if (timeout_detected == TRUE) break; \
	} \
	  char imrLen = 0; \
	imrLen = ((iActualRXStatus & 0b0111000) >> 3); \
	length = imrLen; \
	LP = ((iActualRXStatus & CPLD_RX_STATUS_LP) != 0) ? 1 : 0; \
	BC = ((iActualRXStatus & CPLD_RX_STATUS_BC) != 0) ? 1 : 0; \
	OPTIMIZED__AVR32_CPLD_Read(CPLD_ADDRESS_SENDERS_ADRS, &senders_address); \
	OPTIMIZED__AVR32_CPLD_BurstRxRead(data, CPLD_ADDRESS_RX_BUF_BEGIN); \
	OPTIMIZED__AVR32_CPLD_Write(CPLD_ADDRESS_RX_CONTROL, CPLD_RX_CONTROL_CLEAR); \
	break; \
	} \
}) 

*/



void MACRO_XLINK_wait_packet(char* data, unsigned int length, char time_out, char timeout_detected, char senders_address, char LP, char BC)
{
/*({ \
		while(TRUE) \
		{ \
			BC = 0; \
			LP = 0; \
			timeout_detected = FALSE; \
			length = 0; \
			senders_address = 0; \
			  char iActualRXStatus = 0; \
			  unsigned char us1 = 0; \
			  unsigned char us2 = 0; \
			MACRO_XLINK_get_RX_status(iActualRXStatus); \
			u32 iTimeoutHolder; \
			MACRO_GetTickCount(iTimeoutHolder); \
			u32 iTickHolder; \
			if ((iActualRXStatus & CPLD_RX_STATUS_DATA) == 0) \
			{ \
				while (TRUE) \ 
				{ \
					MACRO_XLINK_get_RX_status(iActualRXStatus); \
					if ((iActualRXStatus & CPLD_RX_STATUS_DATA) != 0) break; \
					MACRO_GetTickCount(iTickHolder); \
					if ((iTickHolder - iTimeoutHolder) > time_out) \
					{ \
						timeout_detected = TRUE; \
						length = 0; \
						senders_address = 0; \
						break; \
					}  \
				} \				
				if (timeout_detected == TRUE) break; \
			} \
			  char imrLen = 0; \
			imrLen = ((iActualRXStatus & 0b0111000) >> 3); \
			length = imrLen; \
			LP = ((iActualRXStatus & CPLD_RX_STATUS_LP) != 0) ? 1 : 0; \
			BC = ((iActualRXStatus & CPLD_RX_STATUS_BC) != 0) ? 1 : 0; \
			OPTIMIZED__AVR32_CPLD_Read(CPLD_ADDRESS_SENDERS_ADRS, &senders_address); \
			OPTIMIZED__AVR32_CPLD_BurstRxRead(data, CPLD_ADDRESS_RX_BUF_BEGIN); \
			OPTIMIZED__AVR32_CPLD_Write(CPLD_ADDRESS_RX_CONTROL, CPLD_RX_CONTROL_CLEAR); \
			break; \
		} \
	}) */

	}



#endif


/////////////////////////////////////////////////////////////////
#endif /* AVR32_OPTIMIZEDTEMPLATES_H_ */
