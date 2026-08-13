/**
 * @file    tick_timer.h
 * @brief   Periodic system tick built on TIM2.
 *
 * Provides the single time base the application runs on. The interrupt only
 * raises a flag; all work happens in the main loop, so no application logic
 * ever executes in interrupt context.
 */

#ifndef TICK_TIMER_H
#define TICK_TIMER_H

#include <stdbool.h>

/** Start TIM2 ticking at APP_TICK_HZ. */
void TickTimer_init(void);

/**
 * Consume the "a tick elapsed" flag.
 *
 * @return true exactly once per elapsed tick period; false afterwards.
 */
bool TickTimer_hasElapsed(void);

#endif /* TICK_TIMER_H */
