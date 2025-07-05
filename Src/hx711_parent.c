/*
 * hx711_parent.c
 *
 *  Created on: Apr 29, 2025
 *      Author: michaelkarvat
 */


// hx711_parent.c

#include "stm32f303xe.h"
#include <stdio.h>
#include "types.h"
#include "hx711_parent.h"

// Short delay routine (used for generating proper pulse timing)
void hx711_parent_delay(volatile int count)
{
    while(count--);
}

void hx711_parent_gpio_init(void)
{
    // 1) Enable GPIOB clock (bit 18 in AHBENR)
    RCC->AHBENR |= (1U << 18);

    // 2) Configure PB2 as output (PD_SCK)
    GPIOB->MODER &= ~(3U << (2 * 2));
    GPIOB->MODER |=  (1U << (2 * 2));

    // 3) Configure PB3 as input (DOUT)
    GPIOB->MODER &= ~(3U << (3 * 2));
}

unsigned long HX711_Parent_Read(void)
{
    unsigned long count = 0;
    uint8_t i;

    // 1) Wait until DOUT goes LOW (data ready)
    while ((GPIOB->IDR & HX711_PARENT_DOUT_PIN) != 0);

    // 2) Read 24 bits
    for (i = 0; i < 24; i++)
    {
        // PD_SCK high: start bit
        GPIOB->ODR |= HX711_PARENT_PD_SCK_PIN;
        //hx711_parent_delay(10);

        count <<= 1;

        // PD_SCK low: complete clock cycle
        GPIOB->ODR &= ~HX711_PARENT_PD_SCK_PIN;
        //hx711_parent_delay(10);

        // If DOUT is high, set LSB
        if (GPIOB->IDR & HX711_PARENT_DOUT_PIN)
        {
            count++;
        }
    }

    // 3) One extra pulse to set gain/channel (A128)
    GPIOB->ODR |= HX711_PARENT_PD_SCK_PIN;
    //hx711_parent_delay(10);
    GPIOB->ODR &= ~HX711_PARENT_PD_SCK_PIN;
    //hx711_parent_delay(10);

    // 4) Convert from 24-bit two's complement
    count ^= 0x800000;

    return count;
}
