#include "event_manager.h"
//#include "processing.h"
#include "usart2.h" // Add this line
#include "usart1.h"
#include <stdio.h>
#include "types.h"
//#include "terminal.h"
#include "timer2.h"
#include "scheduler.h"

void event_manager_handler(void) {
    printf("Event manager handler\n");

    int i = 0;
    while (1) {

        if (USART2_commandReceived()) {
            TERMINAL_handleCommand();
        }

        if (TIMER2_expired()) {
            SCHEDULER_handle();
        }

        //received command from the sim.
        if (USART1_commandReceived()){
        	char response[64];
			USART1_getCommand(response);
			print("SIMCom => %s\n", response);
		}
    }
}
