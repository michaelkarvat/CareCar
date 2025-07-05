// terminal.c
#include <string.h>
#include "terminal.h"
#include "usart2.h"
#include "scheduler.h"

static char A_Command_Buffer[TERMINAL_SIZE_OF_COMMAND_BUFFER];

void TERMINAL_handleCommand(void)
{
    USART2_getCommand(A_Command_Buffer);
    print("<=+=+= %s =+=+=>\n", A_Command_Buffer);

    const char *prefix = "setnum ";
    if (strncmp(A_Command_Buffer, prefix, strlen(prefix)) == 0) {
        char *newNum = A_Command_Buffer + strlen(prefix);

        // Build a formatted number with leading '+'
        char formatted[PHONE_NUMBER_MAX_LEN];
        if (newNum[0] != '+') {
            // prepend '+'
            snprintf(formatted, sizeof(formatted), "+%s", newNum);
        } else {
            // user somehow included it already
            snprintf(formatted, sizeof(formatted), "%s", newNum);
        }

        // Copy into the global phone buffer
        strncpy(g_phoneNumber, formatted, PHONE_NUMBER_MAX_LEN - 1);
        g_phoneNumber[PHONE_NUMBER_MAX_LEN - 1] = '\0';

        print("Phone number set to %s\n", g_phoneNumber);
    }
}
