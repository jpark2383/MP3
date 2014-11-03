/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _I8259_H
#define _I8259_H

#include "types.h"

/* Ports that each PIC sits on */
#define MASTER_8259_PORT 0x20
#define MASTER_8259_IMR  0x21
#define MASTER_8259_ISR MASTER_8259_PORT
#define MASTER_8259_POLL MASTER_8259_ISR
#define MASTER_8259_OCW3 MASTER_8259_ISR
#define SLAVE_8259_PORT  0xA0
#define SLAVE_8259_IMR   0xA1

/* Number of ports on a single pic */
#define PORTS 8

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */
#define ICW1    0x11
#define ICW2_MASTER   0x20
#define ICW2_SLAVE    0x28
#define ICW3_MASTER   0x04
#define ICW3_SLAVE    0x02
#define ICW4          0x01

/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI             0x60
#define PIC_EOI			0x20
#define ALL_MASK 		0xFF
#define LONG_MASK 		0xFF00
#define PIC_1 			0x01
#define PIC_2 			0x02
#define PIC_3 			0x03
#define PIC_4 			0x04
#define PIC_5 			0x05
#define PIC_6 			0x06
#define PIC_7 			0x07
#define PIC_8 			0x08
#define PIC_9 			0x09
#define PIC_10 			0x010
#define PIC_11 			0x011
#define PIC_12			0x012
#define PIC_13			0x013
#define PIC_14			0x014
#define PIC_15 			0x015
/* Externally-visible functions */

/* Initialize both PICs */
void i8259_init(void);
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num);
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num);
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num);

#endif /* _I8259_H */
