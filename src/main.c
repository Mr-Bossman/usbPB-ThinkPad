#include "common.h"
#include "i2c.h"
#include "uart.h"
#include "fusb302.h"
#include "usb_pd.h"
#include "printf_uart.h"
#include "pit.h"

void timer_callback(void)
{

}

int main(void){
	/* Set F_CPU to 20MHz */
	CPU_CCP = 0xD8;
	CLKCTRL_MCLKCTRLA = CLKCTRL_CLKSEL_OSC20M_gc;
	CPU_CCP = 0xD8;
	CLKCTRL_MCLKCTRLB = 0;
	/* set PORTB as output */
	PORTB.DIRSET = 0xFF;
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

ISR(PORTB_PORT_vect){
	if(PORTB.INTFLAGS & PORT_INT4_bm) {
		PORTB.INTFLAGS = PORT_INT4_bm;
		fusb302_IRQ();
	}
}
