/* i8259.h - Defines used in idt.c
 * controller
 * vim:ts=4 noexpandtab
 */
 
#ifndef _IDT_H
#define _IDT_H

#define six_bit_mask 	0x1F
#define zero_mask 		0
#define one_mask		1
#define zero_dpl		0
#define three_dpl		3

void set_trap_gate(uint32_t num, uint32_t address);
void set_intr_gate(uint32_t num, uint32_t address);

extern void init_idt();

extern void divide_by_0();
extern void debug();
extern void non_mask_intr();
extern void breakpoint();
extern void overflow();
extern void out_of_bounds();
extern void invalid_op();
extern void no_coprocessor();
extern void double_fault();
extern void coprocessor_segment_overrun();
extern void bad_TSS();
extern void no_segment();
extern void stack_fault();
extern void general_protection();
extern void page_fault();
extern void coprocessor_fault();
extern void alignment_check();
extern void machine_check();
/* ask about last two lines */
extern void simd_coprocessor_error();
extern void ignore_int();

#endif 
