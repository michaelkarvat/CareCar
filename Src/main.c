#include "stm32f303xe.h"
#include "usart1.h"  // The new code (PA9/PA10 @ 115200)
#include "usart2.h"  // Your existing debug code
#include "types.h"
#include "timer2.h"
#include "hx711.h"
#include "hx711_parent.h"
#include "mhz19.h"


#include "event_manager.h"
//#include "processing.h"


int main()
{

	//inti all the modules
    print("System init start\n");
    // 1) Init debug USART2 (PC connection).
    USART2_init();
    print("USART2 (PC) initialized.\n");

    // 2) Init USART1 for SIMCom (PA9/PA10, 115200).
    USART1_init();
    print("USART1 (SIMCom) initialized at 115200.\n");

    TIMER2_init();
    hx711_gpio_init();
    hx711_parent_gpio_init();

    // 3) Send basic "AT" command to the SIMCom.
    USART1_print("AT\r\n");
    print("Sent 'AT' to SIMCom.\n");
    USART1_print("AT+COPS=?\r\n");
    //AT+COPS=?

    MHZ19_USART1_init();

    /*==============================================*/

    event_manager_handler();

    return 0;
}

