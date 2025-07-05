#ifndef USART1_H
#define USART1_H

#include "types.h"

// Same buffer size approach as your USART2 code:
#define USART1_SIZE_OF_RX_BUFFER    64
#define USART1_SIZE_OF_PRINT_BUFFER 64

// Initializes USART1 (PA9=TX, PA10=RX) at 115200 baud, RX interrupt enabled.
void USART1_init(void);

// Returns TRUE if a command (terminated by '\n') has been received.
BOOL USART1_commandReceived(void);

// Copies the last command (up to '\n') into p_command.
void USART1_getCommand(char *p_command);

// Transmit a single character (blocking).
void USART1_printCharacter(char c);

// Transmit a null-terminated string (blocking).

void USART1_print(const char *fmt, ...);


#endif // USART1_H
