#include "common.h"
#include "i2c.h"
#include "uart.h"
#include "fusb302.h"

int main(void){
	DDRB = 0xFF;
	PORTD = (1<<PD2);
	EICRA = (1<<ISC01);
	EIMSK |= (1<<INT0);
	sei();
	uart_init(9600);
	uint8_t val;
	i2c_init();
	fusb302_init();
	fusb302_start_sink();
	while(1){
		/*
		i2c_read(FUSB302_I2C_SLAVE_ADDR, REG_STATUS0, &val);
		uart_send(val);
		i2c_read(FUSB302_I2C_SLAVE_ADDR, REG_STATUS1, &val);
		uart_send(val);
		i2c_read(FUSB302_I2C_SLAVE_ADDR, REG_STATUS0A, &val);
		uart_send(val);
		i2c_read(FUSB302_I2C_SLAVE_ADDR, REG_STATUS1A, &val);
		uart_send(val);
		uart_send(0xf0);
		*/
	}
	return 0;
}

ISR(INT0_vect){
	PORTB ^= (1<<PB5);
	fusb302_IRQ();
}
