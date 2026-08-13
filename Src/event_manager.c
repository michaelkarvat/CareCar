/**
 * @file    event_manager.c
 * @brief   The firmware's main loop.
 */

#include "debug_uart.h"
#include "event_manager.h"
#include "gsm_modem.h"
#include "scheduler.h"
#include "terminal.h"
#include "tick_timer.h"

void EventManager_run(void)
{
    DebugUart_printf("Event loop running\n");

    for (;;)
    {
        if (DebugUart_hasCommand())
        {
            Terminal_handleConsoleCommand();
        }

        if (TickTimer_hasElapsed())
        {
            Scheduler_tick();
        }

        GsmModem_pollLog();
    }
}
