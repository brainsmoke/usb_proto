#ifndef MILLIS_H
#define MILLIS_H

#include <stdint.h>

#include "config.h"

#ifndef TIMER_MILLIS
#define TIMER_MILLIS TIM14
#endif

#ifndef RCC_TIMER_MILLIS
#define RCC_TIMER_MILLIS RCC_TIM14
#endif

void millis_timer_init(void);
uint16_t millis_u16(void);

#endif // MILLIS_H
