/**
 * @file    gsm_uart.c
 * @brief   Transport to the SIMCom GSM modem over USART3 (PB10/PB11).
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "app_config.h"
#include "gsm_uart.h"
#include "stm32f303xe.h"
#include "uart_hw.h"

#define GSM_UART_PORT       GPIOB
#define GSM_UART_TX_PIN     10U
#define GSM_UART_RX_PIN     11U

static char txBuffer[GSM_UART_TX_BUFFER_SIZE];

/** Line currently being assembled; owned by the ISR. */
static char rxBuffer[GSM_UART_RX_BUFFER_SIZE];
static size_t rxLength = 0U;

/** Completed line handed to the main loop for logging. */
static char pendingLine[GSM_UART_RX_BUFFER_SIZE];
static volatile bool pendingLineReady = false;

static GsmUart_LineHandler lineHandler = NULL;

void GsmUart_init(GsmUart_LineHandler onLineReceived)
{
    lineHandler = onLineReceived;

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    UartHw_configurePinAsUsart(GSM_UART_PORT, GSM_UART_TX_PIN);
    UartHw_configurePinAsUsart(GSM_UART_PORT, GSM_UART_RX_PIN);

    USART3->BRR = UART_BRR_FOR_BAUD(GSM_UART_BAUD);
    USART3->CR1 = UART_CR1_ENABLE_RX_TX_IRQ;

    NVIC_EnableIRQ(USART3_IRQn);
}

void GsmUart_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    const int written = vsnprintf(txBuffer, sizeof(txBuffer), format, args);
    va_end(args);

    if (written > 0)
    {
        UartHw_writeString(USART3, txBuffer);
    }
}

bool GsmUart_takeLine(char *destination)
{
    if (!pendingLineReady)
    {
        return false;
    }

    (void)strncpy(destination, pendingLine, GSM_UART_RX_BUFFER_SIZE - 1U);
    destination[GSM_UART_RX_BUFFER_SIZE - 1U] = '\0';
    pendingLineReady = false;
    return true;
}

/**
 * USART3 receive interrupt: assembles modem output one line at a time.
 *
 * The modem terminates lines with CRLF; CR is discarded so that handlers see
 * a clean NUL-terminated string. Bytes past the end of the buffer are dropped
 * rather than overwriting memory.
 */
void USART3_EXTI28_IRQHandler(void)
{
    if ((USART3->ISR & USART_ISR_RXNE) == 0U)
    {
        return;
    }

    const char received = (char)(USART3->RDR & 0xFFU);

    if (received == '\r')
    {
        return;
    }

    if (received == '\n')
    {
        rxBuffer[rxLength] = '\0';
        rxLength = 0U;

        (void)memcpy(pendingLine, rxBuffer, sizeof(pendingLine));
        pendingLineReady = true;

        if (lineHandler != NULL)
        {
            lineHandler(rxBuffer);
        }
        return;
    }

    if (rxLength < (GSM_UART_RX_BUFFER_SIZE - 1U))
    {
        rxBuffer[rxLength] = received;
        rxLength++;
    }
}
