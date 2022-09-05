#include "common.h"
#include "i2c.h"
#include "uart.h"
#include "fusb302.h"

int main(void){
	DDRB = 0xFF;
	PORTD = (1<<PD2);
	EICRA = (1<<ISC01)|(1<<ISC00);
	EIMSK |= (1<<INT0);
	sei();
	uart_init(9600);
	uint8_t val;
	i2c_init();
	fusb302_init();
	fusb302_start_sink();
	while(1){
		//uart_send(fusb302_check_cc_state());
	}
	return 0;
}

ISR(INT0_vect){
	PORTB ^= (1<<PB5);
	fusb302_IRQ();
}
