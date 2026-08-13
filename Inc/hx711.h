/**
 * @file    hx711.h
 * @brief   Bit-banged driver for the HX711 24-bit load cell amplifier.
 *
 * The HX711 has no addressable bus: each amplifier needs its own clock and
 * data line. The driver is therefore instance based, so the two load cells in
 * this system (child seat and driver seat) share one implementation instead of
 * two near-identical copies.
 *
 * Both amplifiers are wired to GPIOB on this board.
 */

#ifndef HX711_H
#define HX711_H

#include <stdint.h>

/** Pin assignment for one HX711 amplifier. */
typedef struct
{
    uint32_t clockPin;  /**< Port B pin driving PD_SCK (output). */
    uint32_t dataPin;   /**< Port B pin reading DOUT (input). */
} Hx711;

/**
 * Configure the amplifier's clock pin as an output and data pin as an input.
 *
 * @param sensor Pin assignment for the amplifier.
 */
void Hx711_init(const Hx711 *sensor);

/**
 * Clock out one conversion result.
 *
 * @warning Blocks until the amplifier pulls DOUT low to signal data ready.
 *          With no amplifier connected this never returns, which is why the
 *          default build uses simulated weights (see app_config.h).
 *
 * @param sensor Pin assignment for the amplifier.
 * @return Raw 24-bit sample, offset-binary encoded (0x800000 = zero load).
 */
uint32_t Hx711_read(const Hx711 *sensor);

#endif /* HX711_H */
