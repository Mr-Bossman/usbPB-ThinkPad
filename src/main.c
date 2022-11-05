#include "common.h"
#include "i2c.h"
#include "uart.h"
#include "fusb302.h"
#include "usb_pd.h"
#include "printf_uart.h"
#include "pit.h"

void timer_callback(void)
{
	/*if(fusb302_get_state() == 2 && get_timer() > 100){
			fusb302_init();
			fusb302_start_sink();
	}*/

}

int main(void){
	/* Set F_CPU to 20MHz */
	CPU_CCP = 0xD8;
	CLKCTRL_MCLKCTRLA = CLKCTRL_CLKSEL_OSC20M_gc;
	CPU_CCP = 0xD8;
	CLKCTRL_MCLKCTRLB = 0;
	/* set PORTB 2,3 as output */
	PORTB.DIRSET = (3<<2);
	PORTB.PIN0CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
	/* Pullups on PB4 and INT falling edge */
	PORTB.PIN4CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
	uart_init(115200);
	pit_init();
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
		uart_printf("\n\n\rGot source caps! State: %d\n\r", fusb302_get_state());
		struct source_capability *tmp;
		uint8_t num_caps = usb_pd_get_source_caps(&tmp);
		for(int i = 0; i < num_caps; i++) {
			uart_printf("Voltage: %u, Current: %u, Type: %u, Obj_pos: %u\n\r", tmp[i].voltage, tmp[i].max_current, tmp[i].supply_type, tmp[i].obj_pos);
		}
		//usb_pd_reset_source_caps();
		_delay_ms(1000);
	}
	return 0;
}

ISR(PORTB_PORT_vect){
	if(PORTB.INTFLAGS & PORT_INT4_bm) {
		PORTB.INTFLAGS = PORT_INT4_bm;
		fusb302_IRQ();
	}
}
