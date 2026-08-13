/**
 * @file    terminal.h
 * @brief   Text command interpreter shared by the debug console and SMS.
 *
 * Both remote-control paths carry the same one-line command syntax, so they
 * share one interpreter rather than each parsing commands themselves.
 *
 * Supported commands:
 *   setnum <number>   Set the phone number called when an alert is raised.
 */

#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>

/**
 * Parse and execute one command line.
 *
 * @param line NUL-terminated command, without its line terminator.
 * @return true if the line was recognised as a command and executed.
 */
bool Terminal_executeCommand(const char *line);

/** Read one line from the debug console and execute it. */
void Terminal_handleConsoleCommand(void);

#endif /* TERMINAL_H */
