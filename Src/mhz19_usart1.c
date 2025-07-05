#include "mhz19_usart1.h"

static volatile uint8_t g_rxBuffer[MHZ19_FRAME_LENGTH];
static volatile uint8_t g_rxIndex = 0;
static volatile BOOL    g_dataReady = FALSE;

void MHZ19_USART1_init(void)
{
    // Enable GPIOA clock.
    RCC->AHBENR |= 0x00020000;
    // Enable USART1 clock on APB2.
    RCC->APB2ENR |= 0x00004000;
    // Configure PA9 (TX) and PA10 (RX) as alternate function mode.
    GPIOA->MODER |= 0x00280000;
    // Set alternate function AF7 for PA9 and PA10.
    GPIOA->AFR[1] |= 0x00000770;
    // Set baud rate to 9600 @ 8 MHz (833 decimal).
    USART1->BRR = 833;
    // Enable USART1, RX, TX, and RX interrupt.
    USART1->CR1 = 0x2D;
    // Enable USART1 IRQ in NVIC.
    NVIC_EnableIRQ(USART1_IRQn);
}

void MHZ19_USART1_printCharacter(char c)
{
    USART1->TDR = (uint8_t)c;
    while (!(USART1->ISR & 0x80));  // Wait for TXE flag.
}

void MHZ19_USART1_print(const char *p_data)
{
    while (*p_data)
    {
        MHZ19_USART1_printCharacter(*p_data++);
    }
}

BOOL MHZ19_USART1_dataAvailable(void)
{
    return g_dataReady;
}

void MHZ19_USART1_getData(uint8_t *dest)
{
    for (int i = 0; i < MHZ19_FRAME_LENGTH; i++)
    {
        dest[i] = g_rxBuffer[i];
    }
    g_dataReady = FALSE;
}

// USART1 Interrupt Handler
// Make sure the IRQ handler name matches the startup file.
void USART1_EXTI25_IRQHandler(void)
{
    // Reading RDR clears the RXNE flag.
    uint8_t rx_byte = (uint8_t)(USART1->RDR & 0xFF);
    g_rxBuffer[g_rxIndex++] = rx_byte;
    if (g_rxIndex >= MHZ19_FRAME_LENGTH)
    {
        g_dataReady = TRUE;
        g_rxIndex = 0;
    }
}
