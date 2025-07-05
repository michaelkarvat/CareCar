// hx711_parent.h

#ifndef HX711_PARENT_H
#define HX711_PARENT_H

#include "types.h"

// PD_SCK is connected to PB2 (output)
// DOUT    is connected to PB3 (input)
#define HX711_PARENT_PD_SCK_PIN  (1U << 2)  // PB2
#define HX711_PARENT_DOUT_PIN    (1U << 3)  // PB3

// Short delay for timing pulses
void hx711_parent_delay(volatile int count);

// Initialize the GPIOs for the "parent" HX711
void hx711_parent_gpio_init(void);

// Read one sample (24-bit two's-complement) from the parent HX711
unsigned long HX711_Parent_Read(void);

#endif /* HX711_PARENT_H */
