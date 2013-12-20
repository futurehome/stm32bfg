/*
 * USBProtocol_Module.c
 *
 * Created: 13/10/2012 00:55:37
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */ 
#include "stm32f10x.h"

#include "USBProtocol_Module.h"
#include "AVR32X/AVR32_Module.h"
#include "string.h"
#include "std_defs.h"
#include "bf_general.h"
//#include <avr32/io.h>

#define TRUE  1
#define FALSE 0

void init_USB()
{
	// Initialize appropriate IO for USB Communication
	//__AVR32_USB_SetAccess();
	//__AVR32_USB_Initialize();	

	//Do nothing for ARM				fixed by JJ
}


void USB_wait_stream (char* data,
					  unsigned int  *length,    // output
					  unsigned int   max_len,   // input
					  unsigned int  *time_out,  // Timeout variable
					  unsigned char *invalid_data) // Invalid-Data detected
{
	char byte_received = 0;
	char EOS_detected = 0;
	char expected_length = 0;
	char length_detected = FALSE;
	// Initialize length
	*length = 0;
	*invalid_data = FALSE;

	// while we have data (indicated in status register), read it
	// there is a cap of max_len bytes
	while (USB_inbound_USB_data() == FALSE && (*time_out) > 1)
	{
		WATCHDOG_RESET;
		*time_out = *time_out - 1;
	}; // Loop until there is some data

	// Is there timeout? abort in this case
	if (*time_out == 1)
	{
		USB_send_string("exit due to timeout1\n");
		return;
	}

	  //char byte_received = 0;
	  //char EOS_detected = 0;
	  //char expected_length = 0;
	  //char length_detected = FALSE;

	// Now continue until buffer is empty
	while ((*length < max_len) && *time_out-- > 1)
	{
		// Reset WATCHDOG
		WATCHDOG_RESET;
		
		// We read as u32 as there is inbound data, and max-len was not surpassed
		while ((*length < max_len) && 
			   (USB_inbound_USB_data() != FALSE) && 
			   (!EOS_detected))
		{
			// Reset watchdog
			WATCHDOG_RESET;
			
			// Proceed
			byte_received = USB_read_byte();
			
			if (length_detected == FALSE)
			{
				// If this is the first byte we're receiving, then it's our length indicator
				expected_length = byte_received;
				length_detected = TRUE;
			}
			else
			{			
				// We take the byte as u32 as total length is less than maximum length.
				// Like this if we're in a problematic transaction, we won't encounter buffer overrun
				// and at the same time we will clear FTDI's buffer for the newer transactions
				if ((*length) < max_len) data[(*length)] = byte_received;
				*length = *length + 1;				
			}			
			
			// Check for signature
			EOS_detected = (((*length) >= max_len) || ((*length) == expected_length));			
		}

		// Ok, now we check if we have detected the sign already
		if ((*length >= max_len) || (EOS_detected))
		{
			// Read the remainder of bytes in FTDI fifo until it's empty
			while (USB_inbound_USB_data() == TRUE) 
			{ 
				WATCHDOG_RESET;
				byte_received = USB_read_byte(); 
			} 
			break;
		}			 

		// No, so we wait for more inbound data
		while ((USB_inbound_USB_data() == FALSE) && (*time_out > 1)) 
		{ 
			WATCHDOG_RESET;
			*time_out = *time_out - 1;
		} // Loop until there is some data again...
	}

	// OK, we're done at this point. Most likely we'll have our packet, unless
	// there is a timeout, or buffer is bigger than req_size (which means something must've gone wrong)
	if (!EOS_detected)
	{
		*invalid_data = TRUE;
	}	
}



void USB_wait_packet(char* data,
					 unsigned int *length,    // output
					 unsigned int  req_size,  // input
					 unsigned int  max_len,	// input
					 unsigned int *time_out)  // time_out is in/out
{
	// Check for errors
	if (req_size > max_len) return; // Something is wrong...

	// Initialize length
	*length = 0;

	// while we have data (indicated in status register), read it
	// there is a cap of max_len bytes
	while (USB_inbound_USB_data() == 0 && *time_out-- > 1); // Loop until there is some data

	// Is there timeout? abort in this case
	if (*time_out == 1) return;

	// Now continue until buffer is empty
	while ((*length < req_size) && *time_out > 1)
	{
		// Reduce timeout
		*time_out = *time_out - 1;

		// We read as u32 as there is inbound data, and max-len was not surpassed
		while ((*length < max_len) && (USB_inbound_USB_data() != 0))
		{
			data[*length] = USB_read_byte();
			*length = *length + 1;
		}

		// Ok, now we check if we have the minimum req_size data we need or not
		if (*length >= req_size) break;

		// No, so we wait for more inbound data
		while (USB_inbound_USB_data() == 0 && *time_out > 1){ *time_out = *time_out - 1; } // Loop until there is some data again...
	}	

}

void USB_send_string(char* data)
{
	  unsigned int istrlen = strlen(data);
	USB_write_data((u8*)data, (u16)istrlen);
}

u16 USB_write_data (u8* data, u16 length)
{
	//__AVR32_USB_SetAccess();
	//__AVR32_USB_WriteData(data, length);
	//__AVR32_USB_FlushOutputData();
	
	u16 wActualSendLength;
	wActualSendLength = __ARM_UsbSendData( data, length);
	
	return wActualSendLength; // We've written these many bytes
}

void USB_send_immediate(void)
{
	//__AVR32_USB_SetAccess();
	//__AVR32_USB_FlushOutputData();
}

u8 USB_inbound_USB_data(void)
{
	u8 uInf;
	// Set access
	//__AVR32_USB_SetAccess();
	//  uInf = __AVR32_USB_GetInformation();
	uInf = __ARM_USB_GetInformation();
	return ((uInf & 0x01) == (0x01)) ? TRUE : FALSE;
}

void USB_flush_USB_data(void)
{
	//__AVR32_USB_SetAccess();
	//__AVR32_USB_FlushInputData();

	ClearUsbRxBuffer();
}

char USB_outbound_space(void)
{
	char uInf;
	//__AVR32_USB_SetAccess();
	//  uInf = __AVR32_USB_GetInformation();
	  uInf = __ARM_USB_GetInformation();
	return ((uInf & 0x02) == (0x02)) ? TRUE : FALSE;	
}

u8 USB_read_byte(void)
{

	
	// Read a byte
	  u8 uiData = 0;
	  //char uiDataRetCount = 0;
	  	// Set access to USB
	//__AVR32_USB_SetAccess();
	//__AVR32_USB_GetData(&uiData, 1);
	__ARM_UsbGetData(&uiData, 1);
	return uiData;
}

char USB_read_status(void)
{
	//__AVR32_USB_SetAccess();
	return (char)(__ARM_USB_GetInformation());
}

char USB_write_byte(u8 data)
{
	//__AVR32_USB_SetAccess();
	//__AVR32_USB_WriteData(&data,1);
	__ARM_UsbSendData(&data, 1);
	return 1; // By default, since only 1 character is read
}

