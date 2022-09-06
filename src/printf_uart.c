#include "common.h"
#include "printf_uart.h"
#include "uart.h"

char uart_tx_buffer[100];

void uart_puts(const char *s)
{
	while (*s)
		uart_send(*s++);
}

void uart_put(uint8_t* data, size_t len)
{
	while(len--)
		uart_send(*data++);
}
