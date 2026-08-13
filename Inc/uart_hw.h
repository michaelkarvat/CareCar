/**
 * @file    uart_hw.h
 * @brief   Register-level helpers shared by the three UART drivers.
 *
 * The firmware talks to the USART peripherals directly rather than through
 * ST's HAL. Each of the three links (debug console, GSM modem, CO2 sensor)
 * needs the same bring-up sequence and the same blocking transmit primitive,
 * so that logic lives here once instead of being copied three times.
 */

#ifndef UART_HW_H
#define UART_HW_H

#include <stdint.h>

#include "app_config.h"
#include "stm32f303xe.h"

/**
 * Baud rate divisor for the USART BRR register.
 *
 * Valid because every USART runs in oversampling-by-16 mode (OVER8 = 0), where
 * BRR is simply the peripheral clock divided by the requested baud rate.
 */
#define UART_BRR_FOR_BAUD(baud)     ((uint32_t)(APP_PCLK_HZ / (baud)))

/**
 * CR1 value that enables the peripheral, both directions and the RX interrupt.
 * Every link in this project is configured identically: 8N1, RX interrupt
 * driven, blocking TX.
 */
#define UART_CR1_ENABLE_RX_TX_IRQ \
    (USART_CR1_UE | USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE)

/**
 * Switch a GPIO pin to alternate function 7, the USART function on this part.
 *
 * @param port GPIO port the pin belongs to.
 * @param pin  Pin number within the port (0-15).
 */
void UartHw_configurePinAsUsart(GPIO_TypeDef *port, uint32_t pin);

/**
 * Transmit one byte, blocking until the transmit register can accept it.
 *
 * @param uart Peripheral to transmit on.
 * @param byte Byte to send.
 */
void UartHw_writeByte(USART_TypeDef *uart, uint8_t byte);

/**
 * Transmit a NUL-terminated string, blocking until the last byte is queued.
 *
 * @param uart Peripheral to transmit on.
 * @param text String to send; must not be NULL.
 */
void UartHw_writeString(USART_TypeDef *uart, const char *text);

#endif /* UART_HW_H */
