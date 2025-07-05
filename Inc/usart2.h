#ifndef USART2_H
#define USART2_H

#include "types.h"
#include <stdarg.h>

//
// Adjust these to fit your needs
//
#define USART2_SIZE_OF_RX_BUFFER    128
#define USART2_SIZE_OF_PRINT_BUFFER 128

// Simple printf-like function.
void print(char *p_format, ...);

// Has a command terminated with '\n' been received?
BOOL USART2_commandReceived(void);

// Copies the most recently received command (up to '\n') into p_command.
void USART2_getCommand(char *p_command);

// Sets up USART2 for 9600 baud on PA2/PA3 and enables interrupts.
void USART2_init(void);

// Transmit a single character (blocking).
void USART2_printCharacter(char c);

// Transmit a null-terminated string (blocking).
void USART2_print(const char *p_data);

#endif // USART2_H
