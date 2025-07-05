// scheduler.c
/*
 * scheduler.c
 *  Created on: Apr 23, 2025
 *      Author: michaelkarvat
 */

#include "scheduler.h"
#include "types.h"
#include "usart2.h"
#include "processing.h"
#include "usart1.h"
#include <stdio.h>   // for snprintf

// The phone‐number string, defaulting to the original number
char g_phoneNumber[PHONE_NUMBER_MAX_LEN] = "+972545902372";

// How many seconds to wait before hanging up
#define ALERT_SUPPRESSION_SECONDS 20

// Scheduler internal states
typedef enum {
    SCHED_STATE_REGULAR,  // Normal operation: check ALG_handle()
    SCHED_STATE_CONTACT,  // Send the alert
    SCHED_STATE_DELAY     // Wait then hang up
} SchedState;

// Current state and counter
static SchedState schedState = SCHED_STATE_REGULAR;
static int        suppressionCounter = 0;

/// Send the actual alert (called once in CONTACT state)
void SCHEDULER_send_command(void)
{
    print("******************** Alert: Child left in car! *******************\n");
    // Wake up SIM module
    USART1_print("AT\r\n");
    print("Sent 'AT' to SIMCom.\n");

    // Dial the currently configured number
    char cmd[PHONE_NUMBER_MAX_LEN + 8];
    int n = snprintf(cmd, sizeof(cmd), "ATD%s;\r\n", g_phoneNumber);
    if (n > 0 && n < (int)sizeof(cmd)) {
        USART1_print(cmd);
    } else {
        print("Error: phone number too long\n");
    }
}

/// Hang up an ongoing call
void SCHEDULER_HANG_UP_CALL(void)
{
    print("Scheduler_handle: Hanging up call\n");
    USART1_print("AT+CHUP\r\n");
}

/// This runs once per second (driven by event_manager → TIMER2_expired)
void SCHEDULER_handle(void)
{
    switch (schedState)
    {
        case SCHED_STATE_REGULAR:
            // Normal sensor/state processing
            ALG_handle();
            // If ALG signals an alert, move to CONTACT
            if (triger_alert())
            {
                schedState = SCHED_STATE_CONTACT;
            }
            break;

        case SCHED_STATE_CONTACT:
            // Fire off the alert once
            SCHEDULER_send_command();
            // Start the hang-up timer
            suppressionCounter = ALERT_SUPPRESSION_SECONDS;
            schedState = SCHED_STATE_DELAY;
            break;

        case SCHED_STATE_DELAY:
            if (suppressionCounter > 0)
            {
                // Still waiting: decrement and stay in DELAY
                suppressionCounter--;
                print("CALLING : %d seconds remaining\n", suppressionCounter);
            }
            else
            {
                // Time’s up: hang up and return to REGULAR
                SCHEDULER_HANG_UP_CALL();
                schedState = SCHED_STATE_REGULAR;
            }
            break;
    }
}

/// Helper: detect and clear an ALG alert flag
BOOL triger_alert(void)
{
    if (ALG_isAlertTriggered())
    {
        ALG_reset();
        return TRUE;
    }
    return FALSE;
}
