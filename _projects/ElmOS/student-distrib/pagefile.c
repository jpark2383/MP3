#include "pagefile.h"
#include "x86_desc.h"
/*

This function initializes paging  by setting the right entries for each of page table , sets the registers
cr3 , cr4 and cr0

*/
void paging_init()
{
	int i;
	uint32_t cr4 = 0;   //This variable holds the value from the cr4 register
	uint32_t cr0;    ////This variable holds the value from the cr0 register
	
	asm volatile ("mov %%CR4, %0": "=b"(cr4)); 
	
	cr4 = cr4 | CR4_PSE ; //Enable PSE bit
	
	asm volatile ("mov %0, %%CR4":: "b"(cr4));
	
	for(i = 0; i < NUM_ENTRIES_PD; i++)  //initialize entries
	{
		page_directory[i] = EMPTY;
		task1_page_directory[i]= EMPTY;
	}
	
	for(i = 1; i < NUM_ENTRIES_PT; i++)  //initialize entries
	{
		page_table[i] = EMPTY;
		
	}
	
	//kernel
	page_table[VIDEO_MEM_LOCATION] = VIDEO_MEM_PAGE_SU; //Set the video memory location in the address space
	page_directory[0] = (uint32_t) page_table | USER_SUPERVISOR | PRESENT | MASK_RW ;
	page_directory[1] = KERNEL_PTR;
	
	//program 1
	page_table1[VIDEO_MEM_LOCATION] = VIDEO_MEM_PAGE_U; //Set the video memory location in the address space
	task1_page_directory[0] = (uint32_t) page_table1 | USER_SUPERVISOR | PRESENT | MASK_RW ;
	task1_page_directory[1] = KERNEL_PTR;
	task1_page_directory[32]= MB_8_MAP;
	
	//The below code fetches the register values , sets them appropriately and writes them back
	asm volatile ("mov %0, %%CR3":: "b"(page_directory));
	
	asm volatile ("mov %%CR0, %0": "=b"(cr0));
	
	cr0 = cr0 | ENABLE_PAGING;
	
	asm volatile ("mov %0, %%CR0"::"c"(cr0));	
}
