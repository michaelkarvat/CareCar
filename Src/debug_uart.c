/**
 * @file    debug_uart.c
 * @brief   Console link to the host PC over USART2 (PA2/PA3).
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "app_config.h"
#include "debug_uart.h"
#include "stm32f303xe.h"
#include "uart_hw.h"

#define DEBUG_UART_PORT         GPIOA
#define DEBUG_UART_TX_PIN       2U
#define DEBUG_UART_RX_PIN       3U

static char txBuffer[DEBUG_UART_TX_BUFFER_SIZE];
static char rxBuffer[DEBUG_UART_RX_BUFFER_SIZE];

/** Written by the ISR, consumed by DebugUart_hasCommand(). */
static volatile bool commandReceived = false;

/** Next free slot in rxBuffer; owned by the ISR. */
static size_t rxLength = 0U;

void DebugUart_init(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    UartHw_configurePinAsUsart(DEBUG_UART_PORT, DEBUG_UART_TX_PIN);
    UartHw_configurePinAsUsart(DEBUG_UART_PORT, DEBUG_UART_RX_PIN);

    USART2->BRR = UART_BRR_FOR_BAUD(DEBUG_UART_BAUD);
    USART2->CR1 = UART_CR1_ENABLE_RX_TX_IRQ;

    NVIC_EnableIRQ(USART2_IRQn);
}

void DebugUart_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    /* vsnprintf, not vsprintf: a long log line must truncate, not corrupt. */
    const int written = vsnprintf(txBuffer, sizeof(txBuffer), format, args);
    va_end(args);

    if (written > 0)
    {
        UartHw_writeString(USART2, txBuffer);
    }
}

bool DebugUart_hasCommand(void)
{
    if (!commandReceived)
    {
        return false;
    }
    commandReceived = false;
    return true;
}

void DebugUart_readCommand(char *destination)
{
    (void)strncpy(destination, rxBuffer, DEBUG_UART_RX_BUFFER_SIZE - 1U);
    destination[DEBUG_UART_RX_BUFFER_SIZE - 1U] = '\0';
}

/**
 * USART2 receive interrupt: assembles one newline-terminated line at a time.
 *
 * Reading RDR clears the RXNE flag. Bytes past the end of the buffer are
 * dropped so that an over-long line degrades into a truncated command rather
 * than a memory overwrite.
 */
void USART2_EXTI26_IRQHandler(void)
{
    const char received = (char)(USART2->RDR & 0xFFU);

    if (received == '\n')
    {
        rxBuffer[rxLength] = '\0';
        rxLength = 0U;
        commandReceived = true;
        return;
    }

    if (rxLength < (DEBUG_UART_RX_BUFFER_SIZE - 1U))
    {
        rxBuffer[rxLength] = received;
        rxLength++;
    }
}
