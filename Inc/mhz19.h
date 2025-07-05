#ifndef MHZ19_H
#define MHZ19_H

#include <stdint.h>
#include "types.h"

#define MHZ19_FRAME_LENGTH 9

/**
 * Initialize the MH‑Z19 on USART1.
 */
void MHZ19_USART1_init(void);

/**
 * Read one CO₂ & temperature sample from the sensor.
 *
 * @param ppm         Pointer to store the CO₂ reading (ppm).
 * @param temperature Pointer to store the temperature reading (°C).
 * @return TRUE if a valid reading was obtained; FALSE otherwise.
 */
BOOL MHZ19_read(int *ppm, int *temperature);

#endif // MHZ19_H
