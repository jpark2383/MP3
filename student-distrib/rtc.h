/* rtc.h - Defines used in interactions with the RTC
 * controller
 * vim:ts=4 noexpandtab
 */

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
#define RTC_BASE_FREQ 0x0F
#define RTC_WRITE_MASK 0xF0
#define RTC_MIN 2
#define RTC_MAX 1024
#define IRQ2 0x02
#define IRQ8 0x08



extern void rtc_init(); // Initilization of the RTC. Starts at 2
extern void rtc_interrupt(); // Handler for RTC interrupts
extern int32_t rtc_open(const uint8_t* filename);
extern int32_t rtc_close(int32_t fd);
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
extern void rtc_set(int32_t freq); // helper for setting RTC frequency

#endif
