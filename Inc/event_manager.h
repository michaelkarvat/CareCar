/**
 * @file    event_manager.h
 * @brief   The firmware's main loop.
 */

#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

/**
 * Run the event loop forever.
 *
 * Interrupt handlers only set flags; this loop is where every one of them is
 * turned into work, so all application code runs at thread level with no
 * locking and no reentrancy concerns.
 *
 * @note Never returns.
 */
void EventManager_run(void);

#endif /* EVENT_MANAGER_H */
