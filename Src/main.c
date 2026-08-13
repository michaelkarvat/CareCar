/**
 * @file    main.c
 * @brief   CareCar entry point: bring up every subsystem, then run the loop.
 *
 * Initialisation order matters: the debug console comes up first so that the
 * rest of the bring-up sequence is observable, and the tick timer comes up
 * last so that no tick fires before the modules that service it exist.
 */

#include "alert_contact.h"
#include "debug_uart.h"
#include "event_manager.h"
#include "gsm_modem.h"
#include "safety_monitor.h"
#include "scheduler.h"
#include "tick_timer.h"

int main(void)
{
    DebugUart_init();
    DebugUart_printf("\nCareCar starting\n");

    AlertContact_init();
    GsmModem_init();
    DebugUart_printf("GSM modem link up\n");

    SafetyMonitor_init();
    DebugUart_printf("Sensors up\n");

    Scheduler_init();
    TickTimer_init();
    DebugUart_printf("Init complete\n");

    EventManager_run();

    return 0;
}
