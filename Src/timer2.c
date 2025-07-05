/*
 * timer2.c
 *
 *  Created on: Apr 22, 2025
 *      Author: michaelkarvat
 */


#include "stm32f303xe.h"
#include "timer2.h"
#include "types.h"




static BOOL Expired = FALSE;




void TIM2_IRQHandler(void)
{
	// Clean the interrupt flag.
    TIM2->SR &= ~0x00000001;
	// Raise the software flag.
	Expired = TRUE;
}




BOOL TIMER2_expired(void)
{
	if(Expired)
	{
		Expired = FALSE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}




void TIMER2_init(void)
{
    // Enable Timer 2 clock (p. 152 in the datasheet).
    RCC->APB1ENR |= 0x00000001;
    // Set the frequency of Timer 2.
    // Assuming a clock source of 8 MHz (see pp. 127 and 133 in the datasheet),
    // this will set the timer to tick at a rate of 1 Hz.
    TIM2->ARR = 8000000;
    // Enable the Timer 2 interrupt.
    TIM2->DIER = 0x00000001;
    NVIC_EnableIRQ(TIM2_IRQn);
    // Enable Timer 2.
    TIM2->CR1 = 0x00000001;
}

