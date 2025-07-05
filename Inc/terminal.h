#ifndef TERMINAL_H
#define TERMINAL_H

// How big your command buffer is:
#define TERMINAL_SIZE_OF_COMMAND_BUFFER 32

// Processes a command that has just arrived via USART2.
void TERMINAL_handleCommand(void);

#endif // TERMINAL_H
