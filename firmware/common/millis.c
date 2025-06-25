
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "millis.h"

void millis_timer_init(void)
{
	rcc_periph_clock_enable(RCC_TIMER_MILLIS);
	TIM_ARR(TIMER_MILLIS) = 0xffff;
	TIM_PSC(TIMER_MILLIS) = F_CPU/1000;
	TIM_CR1(TIMER_MILLIS) = TIM_CR1_CEN;
}

uint16_t millis_u16(void)
{
	return TIM_CNT(TIMER_MILLIS);
}

