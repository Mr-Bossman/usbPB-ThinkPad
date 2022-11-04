#ifndef __UART_H__
#define __UART_H__

/* Init UART IP */
void uart_init(uint32_t baud);

/* Send byte to UART */
void uart_send(uint8_t data);

#endif /* __UART_H__ */
