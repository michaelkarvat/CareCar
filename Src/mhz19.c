#include "mhz19.h"
#include "mhz19_usart1.h"  // low‑level USART1 routines
#include "usart2.h"        // for debug printing, if you like
#include <string.h>

static const uint8_t MHZ19_Read_Cmd[MHZ19_FRAME_LENGTH] = {
    0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79
};

static uint8_t MHZ19_checksum(const uint8_t *packet) {
    uint8_t sum = 0;
    for (int i = 1; i < 8; i++) {
        sum += packet[i];
    }
    return (uint8_t)(0xFF - sum + 1);
}

BOOL MHZ19_read(int *ppm, int *temperature) {
    // 1) Send the “read CO2” command
    for (int i = 0; i < MHZ19_FRAME_LENGTH; i++) {
        MHZ19_USART1_printCharacter(MHZ19_Read_Cmd[i]);
    }

    // 2) Wait for data from the sensor (via interrupt)
    if (!MHZ19_USART1_dataAvailable()) {
        return FALSE;
    }

    // 3) Grab the frame
    uint8_t response[MHZ19_FRAME_LENGTH];
    MHZ19_USART1_getData(response);

    // 4) Validate header and checksum
    if (response[0] != 0xFF || response[1] != 0x86) {
        return FALSE;
    }
    if (MHZ19_checksum(response) != response[8]) {
        return FALSE;
    }

    // 5) Parse values
    *ppm         = ((int)response[2] << 8) | response[3];
    *temperature = (int)response[4] - 40;

    return TRUE;
}
