/**
 * @file    uart_hw.c
 * @brief   Register-level helpers shared by the three UART drivers.
 */

#include "uart_hw.h"

/** Alternate function index that maps a pin to a USART on the STM32F303RE. */
#define GPIO_AF7_USART          7U

/** Each pin occupies two bits in MODER and four bits in AFR. */
#define MODER_BITS_PER_PIN      2U
#define AFR_BITS_PER_PIN        4U
#define AFR_PINS_PER_REGISTER   8U

#define MODER_ALTERNATE_FUNCTION 0x2U
#define MODER_MASK               0x3U
#define AFR_MASK                 0xFU

void UartHw_configurePinAsUsart(GPIO_TypeDef *port, uint32_t pin)
{
    const uint32_t moderShift = pin * MODER_BITS_PER_PIN;
    port->MODER &= ~(MODER_MASK << moderShift);
    port->MODER |= (MODER_ALTERNATE_FUNCTION << moderShift);

    /* AFR[0] covers pins 0-7 (AFRL) and AFR[1] pins 8-15 (AFRH). */
    const uint32_t afrIndex = pin / AFR_PINS_PER_REGISTER;
    const uint32_t afrShift = (pin % AFR_PINS_PER_REGISTER) * AFR_BITS_PER_PIN;
    port->AFR[afrIndex] &= ~(AFR_MASK << afrShift);
    port->AFR[afrIndex] |= (GPIO_AF7_USART << afrShift);
}

void UartHw_writeByte(USART_TypeDef *uart, uint8_t byte)
{
    while ((uart->ISR & USART_ISR_TXE) == 0U)
    {
        /* Spin until the previous byte has moved into the shift register. */
    }
    uart->TDR = byte;
}

void UartHw_writeString(USART_TypeDef *uart, const char *text)
{
    while (*text != '\0')
    {
        UartHw_writeByte(uart, (uint8_t)*text);
        text++;
    }
}
