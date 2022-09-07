#include "common.h"
#include "watchdog.h"
#include <avr/wdt.h>

void (*wdt_callback)(void) = NULL;

static volatile uint16_t timerlock = 0;
static volatile uint16_t timer = 0;

/* Wait for timeout period *32ms */
bool wait_for_timerlock(uint16_t timeout)
{
	if(timerlock == 0)
		timerlock = timeout+1;
	if(timerlock <= 1) {
		timerlock = 0;
		return false;
	}
	return true;
}

/* Start timer */
void start_timer()
{
	timer = 0;
}

/* Get timer value *32ms */
uint16_t get_timer(void)
{
	return timer;
}

/* Init Watchdog timer */
void watchdog_init() {
	// Set Watchdog prescaler to 32ms and Enable Interrupt
	WDTCSR = (1<<WDP0) | (1<<WDIE);
	wdt_reset();
}

/* Watchdog interrupt */
ISR(WDT_vect) {
	if(timerlock > 0)
		timerlock--;
	timer++;
	if(wdt_callback != NULL)
		wdt_callback();
}
