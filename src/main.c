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
	/* Pullups on INT0 */
	PORTD |= (1<<PD2);
	/* Init INT0 falling EDG */
	EICRA = (1<<ISC01);
	EIMSK |= (1<<INT0);
	uart_init(9600);
	watchdog_init();
	start_timer();
	set_timer_callback(timer_callback);
	sei();
	i2c_init();
	fusb302_init();
	fusb302_start_sink();
	while(1){
		while(usb_pd_get_source_caps(NULL) == 0) {
			uart_printf("\rWaiting for source caps... State: %d, %d", fusb302_get_state(),get_timer());
			_delay_ms(100);
		}
		uart_puts("\nGot source caps!\n");
		struct source_capability *tmp;
		uint8_t num_caps = usb_pd_get_source_caps(&tmp);
		for(int i = 0; i < num_caps; i++) {
			uart_printf("Voltage: %u, Current: %u, Type: %u, Obj_pos: %u\n", tmp[i].voltage, tmp[i].max_current, tmp[i].supply_type, tmp[i].obj_pos);
		}
		usb_pd_reset_source_caps();
	}
	return 0;
}

ISR(INT0_vect){
	fusb302_IRQ();
}
