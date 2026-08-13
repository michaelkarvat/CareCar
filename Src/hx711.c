/**
 * @file    hx711.c
 * @brief   Bit-banged driver for the HX711 24-bit load cell amplifier.
 */

#include <stdbool.h>

#include "hx711.h"
#include "stm32f303xe.h"

/** Both amplifiers are wired to port B. */
#define HX711_PORT              GPIOB

#define HX711_SAMPLE_BITS       24U

/**
 * Clock pulses after the sample select the next conversion's input.
 * 25 total pulses = channel A at gain 128, which is what the load cells use.
 */
#define HX711_GAIN_PULSES       1U

/** The amplifier reports offset binary, so mid-scale is the zero-load point. */
#define HX711_ZERO_LOAD_OFFSET  0x800000UL

#define MODER_BITS_PER_PIN      2U
#define MODER_MASK              0x3U
#define MODER_OUTPUT            0x1U
#define MODER_INPUT             0x0U

static uint32_t pinMask(uint32_t pin)
{
    return 1UL << pin;
}

static void setClock(const Hx711 *sensor, bool high)
{
    if (high)
    {
        HX711_PORT->ODR |= pinMask(sensor->clockPin);
    }
    else
    {
        HX711_PORT->ODR &= ~pinMask(sensor->clockPin);
    }
}

static void pulseClock(const Hx711 *sensor)
{
    setClock(sensor, true);
    setClock(sensor, false);
}

static bool readData(const Hx711 *sensor)
{
    return (HX711_PORT->IDR & pinMask(sensor->dataPin)) != 0U;
}

void Hx711_init(const Hx711 *sensor)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    const uint32_t clockShift = sensor->clockPin * MODER_BITS_PER_PIN;
    HX711_PORT->MODER &= ~(MODER_MASK << clockShift);
    HX711_PORT->MODER |= (MODER_OUTPUT << clockShift);

    const uint32_t dataShift = sensor->dataPin * MODER_BITS_PER_PIN;
    HX711_PORT->MODER &= ~(MODER_MASK << dataShift);
    HX711_PORT->MODER |= (MODER_INPUT << dataShift);
}

uint32_t Hx711_read(const Hx711 *sensor)
{
    /* DOUT stays high until a conversion is ready. */
    while (readData(sensor))
    {
    }

    uint32_t sample = 0UL;
    for (uint8_t bit = 0U; bit < HX711_SAMPLE_BITS; bit++)
    {
        /* Data is presented on the falling edge, MSB first. */
        setClock(sensor, true);
        sample <<= 1;
        setClock(sensor, false);

        if (readData(sensor))
        {
            sample |= 1UL;
        }
    }

    for (uint8_t pulse = 0U; pulse < HX711_GAIN_PULSES; pulse++)
    {
        pulseClock(sensor);
    }

    return sample ^ HX711_ZERO_LOAD_OFFSET;
}
