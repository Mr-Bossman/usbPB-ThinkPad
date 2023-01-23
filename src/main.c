#include "common.h"
#include "i2c.h"
#include "uart.h"
#include "fusb302.h"
#include "usb_pd.h"
#include "printf_uart.h"
#include "pit.h"

void timer_callback(void)
{
	if(fusb302_get_state() == PD_STATE_SNK_WAIT && get_timer() > 20){
			fusb302_init();
	}
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
	PORTC.DIRSET = (7<<1);
	uart_init(115200);
	uart_puts("                 ");
	uart_puts("\e[2J\e[1;1H\n\n");
	pit_init();
	start_timer();
	set_timer_callback(timer_callback);
	sei();
	i2c_init();
	fusb302_init();
	while(1){
		uint8_t intr[3];
		while(usb_pd_get_source_caps(NULL) == 0) {
			PORTC.OUTCLR = (1<<2);
			fusb302_read(REG_STATUS0, &intr[0]);
			fusb302_read(REG_MEASURE, &intr[1]);
			fusb302_read(REG_STATUS1A, &intr[2]);
			uart_printf("\rWaiting for source caps... State: %d, Time: %d. Status: 0x%02x 0x%02x 0x%02x\r", fusb302_get_state(),get_timer(), intr[0], intr[1], intr[2]);
			_delay_ms(100);
			if(fusb302_get_state() == PD_STATE_SRC){
				PORTC.OUTSET = (1<<3);
				PORTC.DIRSET = 1;
			} else {
				PORTC.OUTCLR = (1<<3);
				PORTC.DIRCLR = 1;
			}
		}
		PORTC.OUTSET = (1<<2);
		uart_printf("\n\n\rGot source caps! State: %d\n\r", fusb302_get_state());
		struct source_capability *tmp;
		uint8_t num_caps = usb_pd_get_source_caps(&tmp);
		for(int i = 0; i < num_caps; i++) {
			uart_printf("Voltage: %u, Current: %u, Type: %u, Obj_pos: %u\n\r", tmp[i].voltage, tmp[i].max_current, tmp[i].supply_type, tmp[i].obj_pos);
		}
		int b = 0;
		for(int i = 0; i < num_caps; i++)
			if(tmp[i].voltage > tmp[b].voltage)
				b = i;
		uart_printf("Selecting: %d, Voltage: %d, Current: %d, Wattage: %lu\n\r", b, (tmp[b].voltage/1000), tmp[b].max_current, (uint32_t)(tmp[b].voltage/1000)*(uint32_t)tmp[b].max_current);
		usb_pd_request_power(5000,3000);
		/* Enable sink fet*/
		PORTC.OUTSET = (1<<1);
		while(usb_pd_get_source_caps(NULL) != 0);
		uart_printf("\n\rPSU disconnected!\n\r");
		/* Dissable sink fet*/
		PORTC.OUTCLR = (1<<1);
	}
	return 0;
}

ISR(PORTB_PORT_vect){
	PORTB.PIN4CTRL = PORT_PULLUPEN_bm;
	if(PORTB.INTFLAGS & PORT_INT4_bm) {
		while(!(PORTB.IN&(1<<4))) {
			fusb302_IRQ();
			PORTB.INTFLAGS = PORT_INT4_bm;
		}
	}
	PORTB.PIN4CTRL = PORT_PULLUPEN_bm | PORT_ISC_LEVEL_gc;
}
