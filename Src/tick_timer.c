/**
 * @file    tick_timer.c
 * @brief   Periodic system tick built on TIM2.
 */

#include "app_config.h"
#include "stm32f303xe.h"
#include "tick_timer.h"

static volatile bool tickElapsed = false;

void TickTimer_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* No prescaler: TIM2 counts peripheral clock cycles directly, and a 32-bit
     * counter is wide enough to reach a full second at APP_PCLK_HZ. */
    TIM2->ARR = APP_PCLK_HZ / APP_TICK_HZ;
    TIM2->DIER = TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM2->CR1 = TIM_CR1_CEN;
}

bool TickTimer_hasElapsed(void)
{
    if (!tickElapsed)
    {
        return false;
    }
    tickElapsed = false;
    return true;
}

void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF;
    tickElapsed = true;
}
