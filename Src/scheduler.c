/**
 * @file    scheduler.c
 * @brief   Drives the sample -> decide -> alert cycle once per system tick.
 *
 * Implemented as a three-state machine rather than a blocking sequence so that
 * the main loop keeps servicing the console and the modem while an alert call
 * is ringing.
 *
 *   MONITORING --alert confirmed--> ALERTING --call placed--> CALLING
 *        ^                                                       |
 *        +--------------------- call duration elapsed ------------+
 */

#include <stdint.h>

#include "alert_contact.h"
#include "app_config.h"
#include "debug_uart.h"
#include "gsm_modem.h"
#include "safety_monitor.h"
#include "scheduler.h"

typedef enum
{
    SCHEDULER_MONITORING,   /**< Sampling sensors, no alert in progress. */
    SCHEDULER_ALERTING,     /**< Alert confirmed, place the call this tick. */
    SCHEDULER_CALLING       /**< Call in progress, waiting to hang up. */
} SchedulerState;

static SchedulerState state = SCHEDULER_MONITORING;

/** Ticks left before the alert call is terminated. */
static uint8_t callTicksRemaining = 0U;

/**
 * Consume a confirmed alert.
 *
 * The monitor is reset here so that it starts a fresh debounce window instead
 * of re-triggering on the same event while the call is still ringing.
 */
static bool takeConfirmedAlert(void)
{
    if (!SafetyMonitor_isAlertActive())
    {
        return false;
    }
    SafetyMonitor_reset();
    return true;
}

static void raiseAlert(void)
{
    DebugUart_printf("*** ALERT: child left in vehicle ***\n");

    if (!AlertContact_isConfigured())
    {
        DebugUart_printf("No alert number configured; send 'setnum <number>'\n");
        return;
    }

    DebugUart_printf("Calling %s\n", AlertContact_get());
    GsmModem_placeCall(AlertContact_get());
}

void Scheduler_init(void)
{
    state = SCHEDULER_MONITORING;
    callTicksRemaining = 0U;
}

void Scheduler_tick(void)
{
    switch (state)
    {
    case SCHEDULER_MONITORING:
        SafetyMonitor_update();
        if (takeConfirmedAlert())
        {
            state = SCHEDULER_ALERTING;
        }
        break;

    case SCHEDULER_ALERTING:
        raiseAlert();
        callTicksRemaining = ALERT_CALL_DURATION_TICKS;
        state = SCHEDULER_CALLING;
        break;

    case SCHEDULER_CALLING:
        if (callTicksRemaining > 0U)
        {
            callTicksRemaining--;
            DebugUart_printf("Calling: %u ticks remaining\n",
                             (unsigned)callTicksRemaining);
        }
        else
        {
            DebugUart_printf("Hanging up\n");
            GsmModem_hangUp();
            state = SCHEDULER_MONITORING;
        }
        break;

    default:
        state = SCHEDULER_MONITORING;
        break;
    }
}
