#ifndef TIMERS_H_
#define TIMERS_H_

#include <avr/io.h>


/**
 * Initialize Timer 0 (8-bit) in Fast PWM mode.
 */
void Timer0_init_pwm();

/**
 * Initialize Timer 1 in Fast PWM mode (8 bits).
 */
void Timer1_init_pwm();

/**
 * Initialize Timer 1 in CTC mode for accurate tone playback.
 */
void Timer1_init_ctc();

/**
 * Initializes Timer2 for systick interrupt once every 1ms,
 * plus any custom behavior you require.
 */
void Timer2_init_systicks();

/** Declares the system ticks counter (milliseconds since boot) for use within
 * other .c modules! */
extern volatile unsigned int systicks;

/** Useful macro to check whether a specific amount of time passed since some
 * given last event */
#define SYSTICKS_PASSED(last_event, diff_amount) \
	((systicks - (last_event)) >= (diff_amount))

#endif // TIMERS_H_

