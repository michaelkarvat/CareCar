
/* =================== HX711 Code =================== */
#include "stm32f303xe.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "hx711.h"




// Short delay routine (used for generating proper pulse timing)
void hx711_delay(volatile int count)
{
    while(count--);
}

void hx711_gpio_init(void)
{
    // Enable GPIOB clock (bit 18 in AHBENR)
    RCC->AHBENR |= (1 << 18);
    // Configure PB0 as output (for PD_SCK)
    GPIOB->MODER &= ~(3 << (0*2));
    GPIOB->MODER |=  (1 << (0*2));
    // Configure PB1 as input (for DOUT)
    GPIOB->MODER &= ~(3 << (1*2));
}

unsigned long HX711_Read(void)
{
    unsigned long count = 0;
    uint8_t i;

    // Wait until DOUT goes LOW (data ready)
    while((GPIOB->IDR & HX711_DOUT_PIN) != 0);

    // Read 24 bits from HX711
    for(i = 0; i < 24; i++)
    {
        // PD_SCK high: start bit reading
        GPIOB->ODR |= HX711_PD_SCK_PIN;
        //hx711_delay(10);

        count = count << 1;

        // PD_SCK low: complete the clock cycle
        GPIOB->ODR &= ~HX711_PD_SCK_PIN;
        //hx711_delay(10);

        // If DOUT is high, set the least-significant bit
        if(GPIOB->IDR & HX711_DOUT_PIN)
        {
            count++;
        }
    }

    // Send one extra pulse to set gain/channel (25 pulses = Channel A, gain = 128)
    GPIOB->ODR |= HX711_PD_SCK_PIN;
    //hx711_delay(10);
    GPIOB->ODR &= ~HX711_PD_SCK_PIN;
    //hx711_delay(10);

    // Convert 24-bit two's complement to proper value
    count ^= 0x800000;

    return count;
}
