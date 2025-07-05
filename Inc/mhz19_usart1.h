#ifndef MHZ19_USART1_H
#define MHZ19_USART1_H

#include "types.h"
#include "stm32f303xe.h"
#include <stdint.h>

#define MHZ19_FRAME_LENGTH 9

// Initialize USART1 for MH‑Z19 sensor communication.
void MHZ19_USART1_init(void);

// Low-level transmit functions.
void MHZ19_USART1_printCharacter(char c);
void MHZ19_USART1_print(const char *p_data);

// Data retrieval functions for the 9-byte sensor frame.
BOOL MHZ19_USART1_dataAvailable(void);
void MHZ19_USART1_getData(uint8_t *dest);

#endif /* MHZ19_USART1_H */
