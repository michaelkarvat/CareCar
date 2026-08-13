/**
 * @file    mhz19.h
 * @brief   MH-Z19 CO2 sensor protocol: request, validate and decode a reading.
 */

#ifndef MHZ19_H
#define MHZ19_H

#include <stdbool.h>

/** One decoded sample from the sensor. */
typedef struct
{
    int co2Ppm;         /**< CO2 concentration in parts per million. */
    int temperatureC;   /**< Cabin temperature in degrees Celsius. */
} Mhz19_Reading;

/** Bring up the sensor's serial link. */
void Mhz19_init(void);

/**
 * Request one sample and decode the response.
 *
 * The frame is rejected unless the header matches the expected command echo
 * and the trailing checksum is correct, so a partially received or corrupted
 * frame is reported as a failure rather than a plausible-looking reading.
 *
 * @param[out] reading Populated only when the function returns true.
 * @return true if a valid frame was decoded.
 */
bool Mhz19_read(Mhz19_Reading *reading);

#endif /* MHZ19_H */
