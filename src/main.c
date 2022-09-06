#include "common.h"
#include "i2c.h"
#include "uart.h"
#include "fusb302.h"
#include "usb_pd.h"
#include "printf_uart.h"

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
		_delay_ms(500);
		uart_printf("\033[2J\033[HWaiting... %u so far.\n", num_source_caps);
		for(uint8_t i = 0; i < num_source_caps; i++){
			uart_puts("Source Capabilities: \n");
			uart_puts("Supply Type: ");
			uart_printf("%u\n", source_caps[i].supply_type);
			uart_puts(" Object Position: ");
			uart_printf("%u\n", source_caps[i].obj_pos);
			uart_puts(" Max Current: ");
			uart_printf("%u\n", source_caps[i].max_current);
			uart_puts(" Voltage: ");
			uart_printf("%u\n", source_caps[i].voltage);
		}
	}
	return 0;
}

ISR(INT0_vect){
	PORTB ^= (1<<PB5);
	fusb302_IRQ();
}
