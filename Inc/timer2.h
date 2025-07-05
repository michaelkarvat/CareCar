#ifndef TIMER2_H
#define TIMER2_H

#include "types.h"

// Initialize Timer2 so that it interrupts every 1 second (as in your code).
void TIMER2_init(void);

// Returns TRUE if the timer interrupt has occurred since last read.
BOOL TIMER2_expired(void);

#endif // TIMER2_H
