/**
 * @file    safety_monitor.h
 * @brief   Decides whether a child has been left alone in an unsafe cabin.
 *
 * Owns the sensors and the debounced decision state. It never talks to the
 * modem: raising the alarm is the scheduler's responsibility, which keeps the
 * safety policy independent of how an alert is delivered.
 */

#ifndef SAFETY_MONITOR_H
#define SAFETY_MONITOR_H

#include <stdbool.h>

/** Bring up the sensors this module owns. */
void SafetyMonitor_init(void);

/**
 * Sample every sensor and re-evaluate the danger condition.
 *
 * Call once per system tick; the debounce window in app_config.h is expressed
 * in ticks, so the call rate defines the confirmation time.
 */
void SafetyMonitor_update(void);

/**
 * @return true once the danger condition has held for ALERT_DEBOUNCE_TICKS.
 */
bool SafetyMonitor_isAlertActive(void);

/** Clear the alert and its debounce counter after an alert has been acted on. */
void SafetyMonitor_reset(void);

#endif /* SAFETY_MONITOR_H */
