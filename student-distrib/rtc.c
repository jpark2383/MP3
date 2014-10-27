/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"	

/*
* rtc_init
*   DESCRIPTION: Initialize the RTC to allow interrupts
*   INPUTS: None
*   OUTPUTS: None
*   RETURN VALUE: None
*   SIDE EFFECTS: Initilizes the rtc to frequency 2Hz
*/ 
void rtc_init(){	
	uint8_t prev_val;					// keep previous value of Register B
	
	outb(RTC_SEL_B, RTC_INDEX_PORT);	// Set index to select Register B
	prev_val = inb(RTC_DATA_PORT);		// Read and save data
	
	outb(RTC_SEL_B, RTC_INDEX_PORT);	// Set index to Register B again
	outb(prev_val | RTC_ENABLE_PIE, RTC_DATA_PORT);	// Enable Interrupt by enabling bit 6 of Reg B (PIE)
	
	cli();
	int rate = 15;
	outb(0x8A, 0x70);		// set index to register A, disable NMI
	char prev = inb(0x71);	// get initial value of register A
	outb(0x8A, 0x70);		// reset index to A
	outb((prev & 0xF0) | rate, 0x71); //write only our rate to A. Note, rate is the bottom 4 bits.
	sti();
	enable_irq(IRQ2);					// turn on IRQ2
	enable_irq(IRQ8);					// turn on IRQ8
}

/*
* rtc_interrupt
*   DESCRIPTION: Handler for RTC interrupts
*   INPUTS: None
*   OUTPUTS: None
*   RETURN VALUE: None
*   SIDE EFFECTS: Initilizes the rtc to frequency 2Hz
*/ 
void rtc_interrupt()
{
	test_interrupts(); // Run test_interrupts
	//putc('s');
	outb(0x0C, 0x70);	// select register C
	inb(0x71);		// throw away data contents

	send_eoi(8); //Send EOIs
	send_eoi(2);
}
