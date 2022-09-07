#include "common.h"
#include "i2c.h"
#include "uart.h"
#include "fusb302.h"
#include "usb_pd.h"
#include "printf_uart.h"
#include "watchdog.h"

void timer_callback(void)
{

}

int main(void){
	DDRB = 0xFF;
	PORTD = (1<<PD2);
	EICRA = (1<<ISC01);
	EIMSK |= (1<<INT0);
	uart_init(9600);
	watchdog_init();
	start_timer();
	wdt_callback = timer_callback;
	sei();
	i2c_init();
	fusb302_init();
	fusb302_start_sink();
	while(1){
		uart_printf("\033[2J\033[HWaiting... %u so far. state %d\n", num_source_caps,state);
		for(uint8_t i = 0; i < num_source_caps; i++){
			PORTB = (1<<PB5);
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
		_delay_ms(500);
	}
	return 0;
}

ISR(INT0_vect){
	fusb302_IRQ();
}
