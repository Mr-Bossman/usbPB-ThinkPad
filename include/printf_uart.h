#ifndef __PRINTF_UART_H__
#define __PRINTF_UART_H__
#include <common.h>

extern uint8_t uart_tx_buffer[100];

void uart_puts(const char *s);
void uart_put(uint8_t* data, size_t len);

#define uart_printf(...) do { \
	sprintf(uart_tx_buffer, __VA_ARGS__); \
	uart_puts((char*)uart_tx_buffer); \
	} while(0)

#endif /* __PRINTF_UART_H__ */
