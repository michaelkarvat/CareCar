#ifndef HX711_H
#define HX711_H


// PD_SCK is connected to PB0 (output)
// DOUT is connected to PB1 (input)
#define HX711_PD_SCK_PIN  (1 << 0)  // PB0
#define HX711_DOUT_PIN    (1 << 1)  // PB1

// Function Prototypes
void hx711_delay(volatile int count);
void hx711_gpio_init(void);
unsigned long HX711_Read(void);

#endif /* HX711_H */
