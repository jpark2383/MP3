#ifndef TIMER_H
#define TIMER_H

#define PIT_PORT0 0x43
#define PIT_PORT1 0x40
#define PIT_COMMAND 0x36
#define DEFAULT_FREQ 11932
#define HZ_33 36156
#define PIT_IRQ 0
void pit_init(void);

void pit_intr_handler(void);

#endif