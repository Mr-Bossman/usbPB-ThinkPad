#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__


/* Init watchdog timer */
void watchdog_init();

/* Wait for timeout period *32ms */
bool wait_for_timerlock(uint16_t timeout);

/* Start timer */
void start_timer();

/* Get timer value *32ms */
uint16_t get_timer(void);

/* Set callback function for WDT */
void set_timer_callback(void (*callback)(void));

#define break_out(timeout)if (!wait_for_timerlock(timeout)) break

#endif /* __WATCHDOG_H__ */
