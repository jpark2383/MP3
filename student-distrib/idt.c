/* idt.c - Functions to set the IDT
 * vim:ts=4 noexpandtab
 */

#include "x86_desc.h"
#include "types.h"
#include "idt.h"
#include "lib.h"
#include "interrupt_handler.h"
#include "syscall_table.h"
//#include "intr_handler.h"

/* 
 * set_trap_gate
 *   DESCRIPTION: insert a trap gate to idt entry
 *   INPUTS: num - which idt number; 
 *			 address - the address of the handler
 *   RETURN VALUE: none
 *   SIDE EFFECTS: specific idt entry is set
 */

void set_trap_gate(uint32_t num, uint32_t address)
{
	//Set bits to correct values
	idt[num].seg_selector = KERNEL_CS;
	idt[num].reserved4 &= six_bit_mask;
	idt[num].reserved3 = one_mask;
	idt[num].reserved2 = one_mask;
	idt[num].reserved1 = one_mask;
	idt[num].size = one_mask;
	idt[num].reserved0 = zero_mask;
	idt[num].dpl = zero_dpl;
	idt[num].present = one_mask;
	SET_IDT_ENTRY(idt[num], address);
}

/* 
 * set_intr_gate
 *   DESCRIPTION: insert a interrupt gate to idt entry
 *   INPUTS: num - which idt number; 
 *			 address - the address of the handler
 *   RETURN VALUE: none
 *   SIDE EFFECTS: specific idt entry is set
 */

void set_intr_gate(uint32_t num, uint32_t address)
 {
 	//Set bits to correct values
 	idt[num].seg_selector = KERNEL_CS;
	idt[num].reserved4 &= six_bit_mask;
	idt[num].reserved3 = zero_mask;
	idt[num].reserved2 = one_mask;
	idt[num].reserved1 = one_mask;
	idt[num].size = one_mask;
	idt[num].reserved0 = zero_mask;
	idt[num].dpl = zero_dpl;
	idt[num].present = one_mask;
	SET_IDT_ENTRY(idt[num], address);
 }

 /* 
 * set_system_gate
 *   DESCRIPTION: insert a system gate to idt entry
 *   INPUTS: num - which idt number; 
 *			 address - the address of the handler
 *   RETURN VALUE: none
 *   SIDE EFFECTS: specific idt entry is set
 */

void set_system_gate(uint32_t num, uint32_t address)
{
	//Set bits to correct values
	idt[num].seg_selector = KERNEL_CS;
	idt[num].reserved4 &= six_bit_mask;
	idt[num].reserved3 = one_mask;
	idt[num].reserved2 = one_mask;
	idt[num].reserved1 = one_mask;
	idt[num].size = one_mask;
	idt[num].reserved0 = zero_mask;
	idt[num].dpl = three_dpl;
	idt[num].present = one_mask;
	SET_IDT_ENTRY(idt[num], address);
}
/*
 * init_idt
 *   DESCRIPTION: initialize the idt table with pointer to the exceptions, interrupts and system calls
 *   INPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: idt table is set,
 */
void init_idt()
{
	//Set gates depending on exception
	int i =0;
	set_trap_gate(0,(uint32_t)&divide_by_0);
	set_trap_gate(1,(uint32_t)&debug);
	set_intr_gate(2,(uint32_t)&non_mask_intr);
	set_system_gate(3,(uint32_t)&breakpoint);
	set_system_gate(4,(uint32_t)&overflow);
	set_system_gate(5,(uint32_t)&out_of_bounds);
	set_trap_gate(6,(uint32_t)&invalid_op);
	set_trap_gate(7,(uint32_t)&no_coprocessor);
	set_trap_gate(8,(uint32_t)&double_fault);
	set_trap_gate(9,(uint32_t)&coprocessor_segment_overrun);
	set_trap_gate(10,(uint32_t)&bad_TSS);
	set_trap_gate(11,(uint32_t)&no_segment);
	set_trap_gate(12,(uint32_t)&stack_fault);
	set_trap_gate(13,(uint32_t)&general_protection);
	set_intr_gate(14,(uint32_t)&page_fault);
	set_trap_gate(15,(uint32_t)&coprocessor_fault);
	set_trap_gate(16,(uint32_t)&alignment_check);
	set_trap_gate(17,(uint32_t)&machine_check);
	set_trap_gate(18,(uint32_t)&simd_coprocessor_error);
	//set the remaining locations with ignore
	for(i = 19; i < 32; i++)
		set_trap_gate(i, (uint32_t)&ignore_int);
	for(i = 32; i < 256; i++)
		set_intr_gate(i, (uint32_t)&ignore_int);
	set_intr_gate(33, (uint32_t)&keyboard_handler);
	set_intr_gate(40, (uint32_t)&rtc_handler);
	set_system_gate(128, (uint32_t)&syscall_handler);
}
	
// Code for all of the Exceptions
void divide_by_0()
{
	printf("divide_by_0 error exception");
	cli();
	while(1);
}
void debug()
{
	printf("debug error exception");
	cli();
	while(1);
}
void non_mask_intr()
{
	printf("non_mask_intr exception");
	cli();
	while(1);
}
void breakpoint()
{
	printf("breakpoint exception");
	cli();
	while(1);
}
void overflow()
{
	printf("overflow exception");
	cli();
	while(1);
}
void out_of_bounds()
{
	printf("out_of_bounds exception");
	cli();
	while(1);
}
void invalid_op()
{
	printf("invalid_op exception");
	cli();
	while(1);
}
void no_coprocessor()
{
	printf("no_coprocessor exception");
	cli();
	while(1);
}
void double_fault()
{
	printf("double_fault exception");
	cli();
	while(1);
}
void coprocessor_segment_overrun()
{
	printf("coprocessor_segment_overrun exception");
	cli();
	while(1);
}
void bad_TSS()
{
	printf("bad_TSS exception");
	cli();
	while(1);
}
void no_segment()
{
	printf("no_segment exception");
	cli();
	while(1);
}
void stack_fault()
{
	printf("stack_fault exception");
	cli();
	while(1);
}
void general_protection()
{
	printf("general_protection exception");
	cli();
	while(1);
}
void page_fault()
{
	printf("page_fault exception\n");
	cli();
	while(1);
}
void coprocessor_fault()
{
	printf("coprocessor_fault exception");
	cli();
	while(1);
}
void alignment_check()
{
	printf("alignment_check exception");
	cli();
	while(1);
}
void machine_check()
{
	printf("machine_check exception");
	cli();
	while(1);
}
void simd_coprocessor_error()
{
	printf("simd_coprocessor_error exception");
	cli();
	while(1);
}

// Other Reserved for INTEL
void ignore_int()
{
	printf("ignored exceptions. ");
	while(1);
}

