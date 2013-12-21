/*
 * USBProtocol_Module.h
 *
 * Created: 13/10/2012 00:29:33
 *  Author: NASSER GHOSEIRI
 * Company: Butterfly Labs
 */ 


#ifndef USBPROTOCOL_MODULE_H_
#define USBPROTOCOL_MODULE_H_

void	init_USB(void);

void	USB_wait_packet(char* data,
					    unsigned int  *length,    // output
					    unsigned int   req_size,   // input
					    unsigned int   max_len,	  // input
					    unsigned int  *time_out);  // time_out is in/out

void	USB_wait_stream(char* data,
						unsigned int  *length,    // output
						unsigned int   max_len,   // input
						unsigned int  *time_out, // Timeout variable
						unsigned char *invalid_data); // Invalid data detected

void	USB_send_string(char* data);

u16 	USB_write_data (u8* data, u16 length);

void	USB_send_immediate(void);
u8		USB_inbound_USB_data(void);
void	USB_flush_USB_data(void);
char	USB_outbound_space(void);
u8		USB_read_byte(void);
char	USB_read_status(void);
char	USB_write_byte(u8 data);


#endif /* USBPROTOCOL_MODULE_H_ */
