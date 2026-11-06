#ifndef JITTER_TIMER_H
#define JITTER_TIMER_H


#define ROUTING5 (0x300)
#define ROUTING4 (0x100)
#define VALUE   (0x000)

#define HISTOGRAM_MAX (0x80)

#define LED_HIDDEN  (0)
#define LED0  (4)
#define LED1  (8)
#define LED2 (12)
#define LED3 (16)

#define ROUTE5_PACKET (3)
#define PACKET1  (7)
#define PACKET2  (11)
#define PACKET3  (15)

#define BITBANG_NATIVE_CYCLES (70)

//#define MEASURE_LEN            (1024)
#define MEASURE_LEN            (64)
#define MEASURE_PERIODS        (MEASURE_LEN*8-1)
#define MEASURE_NATIVE_CYCLES  (BITBANG_NATIVE_CYCLES*MEASURE_PERIODS)
#define FRAME_LEN              (MEASURE_LEN+PACKET3+1)

#ifndef __ASSEMBLER__

#include <stdint.h>

extern uint32_t histogram[HISTOGRAM_MAX+1];
void bitbang(uint16_t *buf, uint32_t pin, uint32_t gpio, uint32_t timer);

void bitbang_clear(uint32_t gpio, uint32_t mask);

#endif

#endif // JITTER_TIMER_H
