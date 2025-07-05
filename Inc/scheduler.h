#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"  // Make sure to include your types definitions (e.g., BOOL)

/**
 * Sends an alert command (e.g., notifies the parent).
 */
void SCHEDULER_send_command(void);

void SCHEDULER_HANG_UP_CALL(void);

/**
 * Periodically called function that triggers sensor reads
 * and updates the algorithm's state.
 */
void SCHEDULER_handle(void);

/**
 * Checks if an alert condition has been reached.
 * @return TRUE if the alert condition is met.
 */
BOOL triger_alert(void);


#define PHONE_NUMBER_MAX_LEN 20

// Global phone-number string (modifiable via terminal)
extern char g_phoneNumber[PHONE_NUMBER_MAX_LEN];

#endif // SCHEDULER_H
