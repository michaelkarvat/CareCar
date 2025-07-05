#include "types.h"
#include <stdio.h>
#include "hx711.h"    // for HX711_Read()
#include "hx711_parent.h"
#include "mhz19.h"    // for MHZ19_read()
#include "usart2.h"   // for print()
#include "processing.h"

// “Provided” parent weight (no sensor yet).
// Set to 0 to simulate parent absent.
#define PARENT_WEIGHT_PROVIDED  20UL

// Thresholds (tune these as needed)
#define CHILD_WEIGHT_THRESHOLD   5UL    // Above this, assume child is present
#define TEMPERATURE_THRESHOLD    30     // °C
#define CO2_THRESHOLD            600    // ppm
#define ALERT_COUNT_REQUIRED     3      // Debounce count

typedef enum {
    STATE_SAFE,
    STATE_ALERT_TRIGGERED
} SystemState;

static SystemState currentState = STATE_SAFE;
static int alertCounter = 0;

void ALG_handle(void) {
    // 1) Read child weight
    //unsigned long childWeight = HX711_Read();
	unsigned long childWeight = PARENT_WEIGHT_PROVIDED;
    print("Child raw weight: %lu\r\n", childWeight);

    // 2) Use provided parent weight
    //unsigned long driverWeight = HX711_Parent_Read();
    unsigned long driverWeight = PARENT_WEIGHT_PROVIDED;
    print("parent raw weight: %lu\r\n", driverWeight);

    // 3) Read CO₂ & temperature
    int co2_ppm, temp_c;
    if (MHZ19_read(&co2_ppm, &temp_c)) {
        print("CO2 = %d ppm, Temp = %d °C\r\n", co2_ppm, temp_c);
    } else {
        print("MHZ19 read failed\r\n");
    }

    // 4) Evaluate flags
    BOOL parentPresent = (driverWeight > 0) ? TRUE : FALSE;
    BOOL childPresent  = (childWeight >= CHILD_WEIGHT_THRESHOLD) ? TRUE : FALSE;
    BOOL envUnsafe     = (temp_c > TEMPERATURE_THRESHOLD) || (co2_ppm > CO2_THRESHOLD);

    print("Flags: parentPresent=%d, childPresent=%d, envUnsafe=%d\r\n",
          parentPresent, childPresent, envUnsafe);

    // 5) Simplified alert logic
    if (childPresent && (!parentPresent || envUnsafe)) {
        alertCounter++;
        print("Condition met; alertCounter=%d\r\n", alertCounter);
    } else {
        if (alertCounter) {
            alertCounter = 0;
            print("Condition cleared; alertCounter reset\r\n");
        }
    }

    // 6) State transition
    if (alertCounter >= ALERT_COUNT_REQUIRED) {
        if (currentState != STATE_ALERT_TRIGGERED) {
            currentState = STATE_ALERT_TRIGGERED;
            print(" ** Transition to STATE_ALERT_TRIGGERED **\r\n");
        }
    } else {
        if (currentState != STATE_SAFE) {
            currentState = STATE_SAFE;
            print(" ** Transition to STATE_SAFE **\r\n");
        }
    }
}

void ALG_reset(void) {
    currentState = STATE_SAFE;
    alertCounter = 0;
    print("ALG_reset: back to STATE_SAFE, alertCounter=0\r\n");
}

BOOL ALG_isAlertTriggered(void) {
    //print("ALG_isAlertTriggered: state=%d\r\n", currentState);
    return (currentState == STATE_ALERT_TRIGGERED);
}
