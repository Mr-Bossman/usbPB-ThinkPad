#include "common.h"

/* Init UART IP */
void uart_init(uint16_t baud){
	// Set baud rate
	uint16_t tmp = (F_CPU/16UL)/baud-1;
	UBRR0H = (uint8_t)(tmp>>8);
	UBRR0L = (uint8_t)tmp;
	// Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Set frame format: 8data, 2stop bit
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

/* Send byte to UART */
void uart_send(uint8_t data){
	// Wait for empty transmit buffer
	while(!(UCSR0A & (1<<UDRE0)));
	// Put data into buffer, sends the data
	UDR0 = data;
}
