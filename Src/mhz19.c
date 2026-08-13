/**
 * @file    mhz19.c
 * @brief   MH-Z19 CO2 sensor protocol: request, validate and decode a reading.
 */

#include "co2_uart.h"
#include "mhz19.h"

/** Byte offsets within an MH-Z19 frame. */
#define MHZ19_OFFSET_START          0U
#define MHZ19_OFFSET_COMMAND        1U
#define MHZ19_OFFSET_CO2_HIGH       2U
#define MHZ19_OFFSET_CO2_LOW        3U
#define MHZ19_OFFSET_TEMPERATURE    4U
#define MHZ19_OFFSET_CHECKSUM       8U

#define MHZ19_START_BYTE            0xFFU
#define MHZ19_COMMAND_READ_CO2      0x86U

/** The sensor encodes temperature with a fixed offset instead of a sign bit. */
#define MHZ19_TEMPERATURE_OFFSET_C  40

/** "Read gas concentration" request, checksum included. */
static const uint8_t readCo2Command[MHZ19_FRAME_LENGTH] = {
    0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79
};

/**
 * Datasheet checksum: negated sum of every byte except the start byte and the
 * checksum byte itself.
 */
static uint8_t computeChecksum(const uint8_t *frame)
{
    uint8_t sum = 0U;
    for (uint8_t i = MHZ19_OFFSET_COMMAND; i < MHZ19_OFFSET_CHECKSUM; i++)
    {
        sum += frame[i];
    }
    return (uint8_t)(0xFFU - sum + 1U);
}

void Mhz19_init(void)
{
    Co2Uart_init();
}

bool Mhz19_read(Mhz19_Reading *reading)
{
    Co2Uart_sendFrame(readCo2Command);

    if (!Co2Uart_hasFrame())
    {
        return false;
    }

    uint8_t frame[MHZ19_FRAME_LENGTH];
    Co2Uart_readFrame(frame);

    const bool headerValid = (frame[MHZ19_OFFSET_START] == MHZ19_START_BYTE) &&
                             (frame[MHZ19_OFFSET_COMMAND] == MHZ19_COMMAND_READ_CO2);
    if (!headerValid)
    {
        return false;
    }

    if (computeChecksum(frame) != frame[MHZ19_OFFSET_CHECKSUM])
    {
        return false;
    }

    reading->co2Ppm = ((int)frame[MHZ19_OFFSET_CO2_HIGH] << 8) |
                      (int)frame[MHZ19_OFFSET_CO2_LOW];
    reading->temperatureC =
        (int)frame[MHZ19_OFFSET_TEMPERATURE] - MHZ19_TEMPERATURE_OFFSET_C;

    return true;
}
