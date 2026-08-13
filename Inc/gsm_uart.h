/**
 * @file    gsm_uart.h
 * @brief   Transport to the SIMCom GSM modem over USART3 (PB10/PB11).
 *
 * This module is only a byte pipe: it brings up the peripheral, sends AT
 * command strings and splits the modem's replies into lines. Interpreting
 * those lines is the job of gsm_modem.c.
 */

#ifndef GSM_UART_H
#define GSM_UART_H

#include <stdbool.h>

/** Longest modem response line that can be buffered, including terminator. */
#define GSM_UART_RX_BUFFER_SIZE     64

/** Longest AT command that can be formatted in one call. */
#define GSM_UART_TX_BUFFER_SIZE     64

/**
 * Callback invoked once per complete line received from the modem.
 *
 * @warning Called from interrupt context, because the unsolicited-result codes
 *          it reacts to must be answered before the modem sends the next line.
 *          The handler must be short and must not block on anything other than
 *          the modem UART itself.
 */
typedef void (*GsmUart_LineHandler)(const char *line);

/**
 * Bring up USART3 at GSM_UART_BAUD with the RX interrupt enabled.
 *
 * @param onLineReceived Handler for incoming lines, or NULL to ignore them.
 */
void GsmUart_init(GsmUart_LineHandler onLineReceived);

/**
 * printf-style transmit to the modem. Blocks until the line is transmitted.
 *
 * @param format Standard printf format string, normally ending in "\r\n".
 */
void GsmUart_printf(const char *format, ...);

/**
 * Take the most recent modem line for logging in the main loop.
 *
 * Kept separate from the interrupt-context handler so that echoing modem
 * traffic to the slow debug console never stalls modem reception.
 *
 * @param[out] destination Buffer of at least GSM_UART_RX_BUFFER_SIZE bytes.
 * @return true if a line was copied out; false if none is pending.
 */
bool GsmUart_takeLine(char *destination);

#endif /* GSM_UART_H */
