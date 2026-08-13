/**
 * @file    terminal.c
 * @brief   Text command interpreter shared by the debug console and SMS.
 */

#include <string.h>

#include "alert_contact.h"
#include "debug_uart.h"
#include "terminal.h"

#define COMMAND_SET_NUMBER      "setnum "

/**
 * Match a command keyword and return the argument that follows it.
 *
 * @param line    Command line to test.
 * @param keyword Keyword including its trailing space.
 * @return Pointer to the argument, or NULL if the line does not match.
 */
static const char *matchCommand(const char *line, const char *keyword)
{
    const size_t keywordLength = strlen(keyword);

    if (strncmp(line, keyword, keywordLength) != 0)
    {
        return NULL;
    }
    return line + keywordLength;
}

bool Terminal_executeCommand(const char *line)
{
    const char *number = matchCommand(line, COMMAND_SET_NUMBER);
    if (number == NULL)
    {
        return false;
    }

    if (!AlertContact_set(number))
    {
        DebugUart_printf("setnum: rejected '%s'\n", number);
        return false;
    }

    DebugUart_printf("Alert number set to %s\n", AlertContact_get());
    return true;
}

void Terminal_handleConsoleCommand(void)
{
    char line[DEBUG_UART_RX_BUFFER_SIZE];

    DebugUart_readCommand(line);
    DebugUart_printf("> %s\n", line);

    if (!Terminal_executeCommand(line))
    {
        DebugUart_printf("Unknown command. Try: setnum <number>\n");
    }
}
