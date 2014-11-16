/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */
#include "rtc.h"	

volatile int32_t intr_flag = 0;

/*
* rtc_init
*   DESCRIPTION: Initialize the RTC to allow interrupts
*   INPUTS: None
*   OUTPUTS: None
*   RETURN VALUE: None
*   SIDE EFFECTS: Initializes the RTC to frequency 2Hz
*/ 
void rtc_init(){	
	uint8_t prev_val;					// keep previous value of Register B
	
	cli();
	outb(RTC_SEL_B, RTC_INDEX_PORT);	// Set index to select Register B
	prev_val = inb(RTC_DATA_PORT);		// Read and save data
	outb(RTC_SEL_B, RTC_INDEX_PORT);	// Set index to Register B again
	outb(prev_val | RTC_ENABLE_PIE, RTC_DATA_PORT);	// Enable Interrupt by enabling bit 6 of Reg B (PIE)
		
	outb(RTC_SEL_A, RTC_INDEX_PORT);		// set index to register A, disable NMI
	prev_val = inb(RTC_DATA_PORT);			// get initial value of register A
	outb(RTC_SEL_A, RTC_INDEX_PORT);		// reset index to A
	outb((prev_val & RTC_WRITE_MASK) | RTC_BASE_FREQ, RTC_DATA_PORT); //write only our rate to A. Note, rate is the bottom 4 bits.
	sti();
	
	enable_irq(IRQ2);					// turn on IRQ2
	enable_irq(IRQ8);					// turn on IRQ8
}

/*
* rtc_open
*   DESCRIPTION: open RTC
*   INPUTS: None
*   OUTPUTS: None
*   RETURN VALUE: None
*   SIDE EFFECTS: None
*/ 
int32_t rtc_open(){
	rtc_init();
	return 0;
}

/*
* rtc_close
*   DESCRIPTION: close RTC
*   INPUTS: None
*   OUTPUTS: None
*   RETURN VALUE: None
*   SIDE EFFECTS: None
*/ 
int32_t rtc_close(){
	return 0;
}

/*
* rtc_read
*   DESCRIPTION: 
*   INPUTS: fd - file descriptor
*			buf - buffer pointer
*			nbytes - number of bytes
*   OUTPUTS: None
*   RETURN VALUE: None
*   SIDE EFFECTS: None
*/ 
int32_t rtc_read(){
	printf("entering RTC_read... \n");
	intr_flag = 0;
	while(intr_flag == 0);
	
	printf("exitting RTC_read... \n");
	return 0;
}

/*
* rtc_write
*   DESCRIPTION: 
*   INPUTS: fd - file descriptor
*			buf - void pointer to Hz to write
*			nbytes - number of bytes to write
*   OUTPUTS: None
*   RETURN VALUE: None
*   SIDE EFFECTS: None
*/ 
int32_t rtc_write(const void* buf, int32_t nbytes){

	if (((const int32_t*)buf) == NULL)
		return -1;
	else if (nbytes != NBYTE_4)
		return -1;
		
	int32_t intr_freq = *((const int32_t*)buf);
	int32_t po2_flag = 0;
	int32_t i;
	
	for (i = RTC_MIN; i <= POW2_MAX; i *= RTC_MIN)
	{
		if (intr_freq == i)
			po2_flag = 1;
	}
	
	if (!po2_flag)
		return -1;
	else 
		rtc_set(intr_freq);
	
	return 0;
}

/*
* rtc_set
*   DESCRIPTION: helper function to set the RTC interrupt frequency
*   INPUTS: None
*   OUTPUTS: None
*   RETURN VALUE: None
*   SIDE EFFECTS: sets interrupt frequency
*/ 
void rtc_set(int32_t freq)
{
	int32_t i;
	uint8_t prev_val;
	uint8_t write_freq = RTC_BASE_FREQ;
	
	if (freq < RTC_MIN)
		freq = RTC_MIN;
	else if ( freq > RTC_MAX)
		freq = RTC_MAX;
	
	for(i = 2; i < freq ; i *= 2){
		write_freq--;
	}
	
	cli();
	outb(RTC_SEL_A, RTC_INDEX_PORT);
	prev_val = inb(RTC_DATA_PORT);	
	outb(RTC_SEL_A, RTC_INDEX_PORT);
	outb((prev_val & RTC_WRITE_MASK) | write_freq, RTC_DATA_PORT);
	sti();	

}


/*
* rtc_interrupt
*   DESCRIPTION: Handler for RTC interrupts
*   INPUTS: None
*   OUTPUTS: None
*   RETURN VALUE: None
*   SIDE EFFECTS: Initializes the rtc to frequency 2Hz
*/ 
void rtc_interrupt()
{
	//test_interrupts(); 					// Run test_interrupts
	outb(RTC_SEL_C, RTC_INDEX_PORT);	// select register C
	inb(RTC_DATA_PORT);					// throw away data contents

	intr_flag++;						// clear 
	//printf("intr_flag is: %d \n", intr_flag);
	
	send_eoi(IRQ8); 					//Send EOIs
	send_eoi(IRQ2);
}
