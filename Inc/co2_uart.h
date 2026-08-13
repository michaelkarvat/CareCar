/**
 * @file    co2_uart.h
 * @brief   Transport to the MH-Z19 CO2 sensor over USART1 (PA9/PA10).
 *
 * The MH-Z19 speaks a fixed-length binary protocol, so this driver collects
 * whole frames rather than lines. Frame contents are decoded by mhz19.c.
 */

#ifndef CO2_UART_H
#define CO2_UART_H

#include <stdbool.h>
#include <stdint.h>

/** Every MH-Z19 request and response is exactly this many bytes. */
#define MHZ19_FRAME_LENGTH      9U

/** Bring up USART1 at CO2_UART_BAUD with the RX interrupt enabled. */
void Co2Uart_init(void);

/**
 * Send one request frame to the sensor. Blocks until the frame is transmitted.
 *
 * @param frame Exactly MHZ19_FRAME_LENGTH bytes.
 */
void Co2Uart_sendFrame(const uint8_t *frame);

/**
 * @return true if a complete response frame is waiting to be read.
 */
bool Co2Uart_hasFrame(void);

/**
 * Copy the buffered response frame out and clear the "frame ready" flag.
 *
 * @param destination Buffer of at least MHZ19_FRAME_LENGTH bytes.
 */
void Co2Uart_readFrame(uint8_t *destination);

#endif /* CO2_UART_H */
