#include "rtc.h"	

/* Initialize the RTC to allow interrupts */
void rtc_init(){	
	uint8_t prev_val;					// keep previous value of Register B
	
	outb(RTC_SEL_B, RTC_INDEX_PORT);	// Set index to select Register B
	prev_val = inb(RTC_DATA_PORT);		// Read and save data
	
	outb(RTC_SEL_B, RTC_INDEX_PORT);	// Set index to Register B again
	outb(prev_val | RTC_ENABLE_PIE, RTC_DATA_PORT);	// Enable Interrupt by enabling bit 6 of Reg B (PIE)
	
	enable_irq(IRQ2);					// turn on IRQ2
	enable_irq(IRQ8);					// turn on IRQ8
}

void rtc_interrupt(){
	putc('s');
	send_eoi(IRQ8);
	send_eoi(IRQ2);
}
