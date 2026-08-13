/**
 * @file    co2_uart.c
 * @brief   Transport to the MH-Z19 CO2 sensor over USART1 (PA9/PA10).
 */

#include "app_config.h"
#include "co2_uart.h"
#include "stm32f303xe.h"
#include "uart_hw.h"

#define CO2_UART_PORT       GPIOA
#define CO2_UART_TX_PIN     9U
#define CO2_UART_RX_PIN     10U

static volatile uint8_t rxFrame[MHZ19_FRAME_LENGTH];
static volatile uint8_t rxLength = 0U;
static volatile bool frameReady = false;

void Co2Uart_init(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    UartHw_configurePinAsUsart(CO2_UART_PORT, CO2_UART_TX_PIN);
    UartHw_configurePinAsUsart(CO2_UART_PORT, CO2_UART_RX_PIN);

    USART1->BRR = UART_BRR_FOR_BAUD(CO2_UART_BAUD);
    USART1->CR1 = UART_CR1_ENABLE_RX_TX_IRQ;

    NVIC_EnableIRQ(USART1_IRQn);
}

void Co2Uart_sendFrame(const uint8_t *frame)
{
    for (uint8_t i = 0U; i < MHZ19_FRAME_LENGTH; i++)
    {
        UartHw_writeByte(USART1, frame[i]);
    }
}

bool Co2Uart_hasFrame(void)
{
    return frameReady;
}

void Co2Uart_readFrame(uint8_t *destination)
{
    for (uint8_t i = 0U; i < MHZ19_FRAME_LENGTH; i++)
    {
        destination[i] = rxFrame[i];
    }
    frameReady = false;
}

/**
 * USART1 receive interrupt: accumulates one fixed-length sensor frame.
 *
 * Reading RDR clears the RXNE flag. The index wraps after a full frame, so a
 * desynchronised stream resynchronises on the next complete frame boundary.
 */
void USART1_EXTI25_IRQHandler(void)
{
    rxFrame[rxLength] = (uint8_t)(USART1->RDR & 0xFFU);
    rxLength++;

    if (rxLength >= MHZ19_FRAME_LENGTH)
    {
        rxLength = 0U;
        frameReady = true;
    }
}
