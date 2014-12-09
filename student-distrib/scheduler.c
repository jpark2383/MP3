#include "scheduler.h"
#include "i8259.h"
#include "systemcalls.h"
#include "lib.h"

int process_mask;

void pit_init(void)
{
	outb(PIT_COMMAND, PIT_PORT0);
	outb(HZ_33 & 0xFF, PIT_PORT1);
	outb(HZ_33 >> 8 , PIT_PORT1);
	enable_irq(PIT_IRQ);
}


void pit_intr_handler(void)
{
	cli();
	send_eoi(PIT_IRQ);
	pcb_t pcb_cur;
	pcb_t pcb_next;
	int pid = find_pid();
	/*if there is only one program running*/
	if(pid == 1)
	{
		return;
	}
	int pid_next = (pid+1)%8;
	pcb_cur = (pcb_t * )(EIGHT_MB - KB_8*pid);
	
	/*save the esp ebp cr3 in pcb*/
	
	/*check if it is next process*/
	/*check if the progress is all full*/
	if(process_mask & 0x7f == 0x7f)
	{
		return;
	}
	/*try to find an empty space for the program*/
	while(pid != pic_next)
	{
		/*if there is a available space*/
		if((process_mask >> pid_next) & 0x1 != 1)
		{
			process_mask |= 0x1 >> pid_next;
			break;
		}
		pid_next = (pic_next + 1) %8;
	}
	/*load the next process's pcb*/
	asm volatile("movl %%cr3, %0" : "=r" (pcb_cur.cr3));
	asm volatile("movl %%esp, %0" : "=r" (pcb_cur.esp));
	asm volatile("movl %%ebp, %0" : "=r" (pcb_cur.ebp));
	
	pcb_next = (pcb_t * )(EIGHT_MB - KB_8*pid_next);
	tss.ss0=KERNEL_DS;
	tss.esp0 = EIGHT_MB - KB_8*(next_pid-1) - 4;
	asm volatile ("mov %0, %%CR3":: "r"(pcb_next.cr3));
	asm volatile ("mov %0, %%esp":: "r"(pcb_next.esp));
	asm volatile ("mov %0, %%ebp":: "r"(pdb_next.ebp));
	asm volatile("leave");
	asm volatile("ret");
}