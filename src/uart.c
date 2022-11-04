#include "common.h"
#include "uart.h"
#include "pit.h"

/* Init UART IP */
void uart_init(uint32_t baud){
	// Set baud rate
	uint16_t tmp = (F_CPU*16ULL)/(uint32_t)baud;
	USART0.BAUD = (tmp+1)>>1;
	// Enable transmitter
	USART0.CTRLB = USART_TXEN_bm | USART_RXMODE_CLK2X_gc;
	// Set frame format: 8data, 2stop bit
	USART0.CTRLC = USART_SBMODE_2BIT_gc | USART_CHSIZE_8BIT_gc;
}

/* Send byte to UART */
void uart_send(uint8_t data){
	// Wait for empty transmit buffer
	while(!(USART0.STATUS & USART_DREIF_bm)) break_out(1);
	// Put data into buffer, sends the data
	USART0.TXDATAL = data;
}
