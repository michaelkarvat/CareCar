#ifndef PROCESSING_H
#define PROCESSING_H


#include "types.h"  // Include basic types such as BOOL

/**
 * Reads sensor values and updates the internal state machine.
 * Should be called periodically (e.g., from the scheduler).
 */
void ALG_handle(void);

/**
 * Resets the algorithm state machine (e.g., after an alert is acknowledged).
 */
void ALG_reset(void);

/**
 * Returns TRUE if the algorithm has determined that an alert condition is met.
 * @return TRUE if an alert is triggered.
 */
BOOL ALG_isAlertTriggered(void);
/*
float processing_read_driver_weight(void);
float processing_read_child_weight(void);
float processing_read_temperature(void);
float processing_read_co2(void);
*/
#endif // PROCESSING_H
