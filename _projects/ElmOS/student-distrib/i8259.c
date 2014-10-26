/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask = ALL_MASK; /* IRQs 0-7 */
uint8_t slave_mask = ALL_MASK; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void)
{
	uint32_t flag;
	cli_and_save(flag);
	outb(ALL_MASK, MASTER_8259_IMR);
	outb(ALL_MASK, SLAVE_8259_IMR);	
	//push the control word to the PIC
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);
	outb(ICW2_MASTER, MASTER_8259_IMR);
	outb(ICW2_SLAVE, SLAVE_8259_IMR);
	outb(ICW3_MASTER, MASTER_8259_IMR);
	outb(ICW3_SLAVE, SLAVE_8259_IMR);
	outb(ICW4, MASTER_8259_IMR);
	outb(ICW4, SLAVE_8259_IMR);
	outb(ALL_MASK, MASTER_8259_IMR);
	outb(ALL_MASK, SLAVE_8259_IMR);
	restore_flags(flag);
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t arg)
{
	unsigned int mask = ~(1 << arg);
	//if greater than 8, slave 
	if(arg & PORTS){
		slave_mask &= ((mask & LONG_MASK)>>PORTS); 
		outb(slave_mask, SLAVE_8259_IMR);
	}
	else{
		master_mask &= (mask & ALL_MASK);
		outb(master_mask, MASTER_8259_IMR);
	}
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t arg)
{
	unsigned int mask = 1 << arg;
	//if greater than 8, slave 
	if(arg & PORTS){
		slave_mask |= (mask & LONG_MASK)>>PORTS; 
		outb(slave_mask, SLAVE_8259_IMR);
	}
	else{
		master_mask |= (mask & ALL_MASK);
		outb(master_mask, MASTER_8259_IMR);
	}
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t arg)
{
	//greater than 8, send eoi to slave
	if(arg & PORTS)
		outb(PIC_EOI, SLAVE_8259_PORT);
	//then send it to master
	outb(PIC_EOI, MASTER_8259_PORT);
}

