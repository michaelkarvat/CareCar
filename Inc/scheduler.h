/**
 * @file    scheduler.h
 * @brief   Drives the sample -> decide -> alert cycle once per system tick.
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

/** Reset the scheduler to its idle state. */
void Scheduler_init(void);

/**
 * Advance the scheduler by one tick.
 *
 * Call from the main loop whenever TickTimer_hasElapsed() returns true.
 */
void Scheduler_tick(void);

#endif /* SCHEDULER_H */
