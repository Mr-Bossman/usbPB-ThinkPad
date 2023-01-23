#include "common.h"
#include "pit.h"

static void (*pit_callback)(void) = NULL;

static volatile uint16_t timerlock = 0;
static volatile uint16_t timer = 0;

void set_timer_callback(void (*callback)(void))
{
	pit_callback = callback;
}

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

/* Enable/Disable Pit timer Interupts */
void pit_IE(uint8_t IE){
	RTC.PITCTRLA = RTC_PERIOD_CYC1024_gc | (IE?RTC_PITEN_bm:0);
}

/* Init Pit timer */
void pit_init() {
	// Set Pit prescaler to 32ms and Enable Interrupt
	RTC.CTRLA = RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm;
	RTC.PITCTRLA = RTC_PERIOD_CYC1024_gc | RTC_PITEN_bm;
	RTC.PITINTCTRL = RTC_PI_bm;
}

/* Pit interrupt */
ISR(RTC_PIT_vect) {
	if(timerlock > 0)
		timerlock--;
	timer++;
	if(pit_callback != NULL)
		pit_callback();
	RTC.PITINTFLAGS = RTC_PI_bm;
}
