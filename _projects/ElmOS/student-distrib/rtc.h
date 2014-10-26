#ifndef _RTC_H
#define _RTC_H

#include "i8259.h"
#include "lib.h"
#include "types.h"

#define RTC_SEL_A 0x8A
#define RTC_SEL_B 0x8B
#define RTC_SEL_C 0x8C
#define RTC_ENABLE_PIE 0x40
#define RTC_INDEX_PORT 0x70
#define RTC_DATA_PORT 0x71
#define IRQ2 0x02
#define IRQ8 0x08

extern void rtc_init();
extern void rtc_interrupt();

#endif
