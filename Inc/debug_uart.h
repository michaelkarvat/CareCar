/**
 * @file    debug_uart.h
 * @brief   Console link to the host PC over USART2 (PA2/PA3).
 *
 * Serves two purposes: it is the firmware's logging sink, and it accepts
 * newline-terminated operator commands (see terminal.h).
 */

#ifndef DEBUG_UART_H
#define DEBUG_UART_H

#include <stdbool.h>

/** Longest single log line; longer messages are truncated, never overflowed. */
#define DEBUG_UART_TX_BUFFER_SIZE   128

/** Longest accepted command line, including the NUL terminator. */
#define DEBUG_UART_RX_BUFFER_SIZE   128

/** Bring up USART2 at DEBUG_UART_BAUD with the RX interrupt enabled. */
void DebugUart_init(void);

/**
 * printf-style logging to the console. Blocks until the line is transmitted.
 *
 * @param format Standard printf format string.
 */
void DebugUart_printf(const char *format, ...);

/**
 * Consume the "a command line has arrived" flag.
 *
 * @return true exactly once per received line; false afterwards.
 */
bool DebugUart_hasCommand(void);

/**
 * Copy the most recently received command line out of the receive buffer.
 *
 * @param destination Buffer of at least DEBUG_UART_RX_BUFFER_SIZE bytes.
 */
void DebugUart_readCommand(char *destination);

#endif /* DEBUG_UART_H */
