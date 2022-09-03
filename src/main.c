#include "common.h"
#include "i2c.h"
#include "uart.h"

int main(void){
	DDRB = 0xFF;
	PORTB = 0x00;
	uart_init(9600);
	uint8_t val;
	i2c_init();
	i2c_read(0x22,&val);
	PORTB = 0xFF;
	while(1){
		uart_send(val);
	}
	return 0;
}
