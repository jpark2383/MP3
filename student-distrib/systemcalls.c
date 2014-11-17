#include "systemcalls.h"
#include "rtc.h"
#include "terminal.h"
#include "filesystem.h"
#include "lib.h"
#include "x86_desc.h"
#include "pagefile.h"

int fd_rtc = 0;
fops_t rtc_fops = {&rtc_open, &rtc_read, &rtc_write, &rtc_close};
fops_t terminal_fops = {&terminal_open, &terminal_read, &terminal_write, &terminal_close};
fops_t filesystem_fops = {&filesystem_open, &filesystem_read, &terminal_write, &filesystem_close};



/*
 * halt
 * 
 * INPUT:
 * OUTPUT:
 * RETURN:
 */
int32_t halt (uint8_t status)
{
	return 0;
}

/*
 * execute
 * 
 * INPUT:
 * OUTPUT:
 * RETURN:
 */
int32_t execute (const uint8_t* command)
{
	uint32_t eip = 0;
	int i;
	if(command == NULL)
		return -1;
	eip = loader(command);
	if(eip == -1)
		return -1;
	pcblock.file_struct[SDIN].flags =1;
	pcblock.file_struct[SDIN].fops_ptr = &terminal_fops;
	pcblock.file_struct[SDOUT].flags =1;
	pcblock.file_struct[SDOUT].fops_ptr = &terminal_fops;
	for(i = 2; i <= 7; i++)
		pcblock.file_struct[i].flags= 0;
	//get the esp
	pcblock.esp = MB_132;
	asm volatile("movl %0, %%esp" : : "r"(pcblock.esp));	
	/*
	asm ("movl %%esp, %0;"
     :"=r"(pcblock.esp)       
     );
	 */
	tss.ss0 = KERNEL_DS;
	tss.esp0 = pcblock.esp;
	asm volatile("              \n\
		cli 				\n\
		movw  %0, %%ax      \n\
		movw %%ax, %%ds		\n\
		pushl %0			\n\
		pushl %1			\n\
		pushfl          	\n\
		#we have to reenable the interrupts at 0x200	\n\
		popl %%eax			\n\
		orl $0x200, %%eax   \n\
		pushl %%eax			\n\
		pushl %2			\n\
		pushl %3			\n\
		iret 				\n\
		"
		: 
		: "g"(USER_DS), "g"(MB_132 - 4), "g"(USER_CS), "g"(eip)
		: "eax", "memory"
	);
	
	
	return 0;
}

/*
 * read()
 * depends on the fd, either call terminal read, or rtc read, 
 * INPUT: fd, fd number; buf, the buffer to pass in; nbytes, the number of bytes to copy into buffer.
 * OUTPUT: on succeed, return 0, else, return -1
 * RETURN: copy into buffer. 
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes)
{
	if(buf == NULL || fd < 0 || fd > FD_MAX || pcblock.file_struct[fd].flags == 0 || nbytes < 0)
	{
		return -1;
	}
	return pcblock.file_struct[fd].fops_ptr->fops_read(fd, buf, nbytes);
}

/*
 * write()
 * depends on the fd, either call terminal write, or rtc write, 
 * INPUT: fd, fd number; buf, the buffer to pass in; nbytes, the number of bytes to copy into buffer.
 * OUTPUT: on succeed, return 0, else, return -1
 * RETURN: display from buffer. 
 */
 
int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{
	if(buf == NULL || fd < 0 || fd > FD_MAX || pcblock.file_struct[fd].flags == 0 || nbytes < 0)
	{
		return -1;
	}
	//printf("got here\n");
	return pcblock.file_struct[fd].fops_ptr->fops_write(fd, buf, nbytes);
}

/*
 * int32_t open(const uint8_t * filename)
 * 
 * INPUT: 
 * OUTPUT: 
 * RETURN: 
 */
 int32_t open (const uint8_t* filename)
 {
//	int taken = 1;
	int pid = 0;
	pid = find_pid();
	uint32_t *pcb_ptr = (uint32_t *)(EIGHT_MB - STACK_EIGHTKB*pid - START);
	//if filename is invalid, return -1
	if(filename == NULL)
	{
		return -1;
	}
	//Iterate through the array to find an unused fd.
	for(fd_index = FD_MIN; fd_index < FILE_ARRAY_SIZE; fd_index++)
	{
		if(pcblock.file_struct[fd_index].flags == 0)
		{
//			taken = 0;
			break;
		}
	}
	pcblock.file_struct[fd_index].flags = 1;
	//run filesystem_open to get the file type
	filesystem_open(filename);
	
	//check the file type
	if(pcblock.dentry[fd_index].file_type == 0)
	{
		fd_rtc = fd_index;
		rtc_open(filename);
		pcblock.file_struct[fd_index].flags = 1;
		pcblock.file_struct[fd_rtc].fops_ptr = &rtc_fops;
		memcpy(pcb_ptr, &pcblock, pcb_size);
		return fd_index;
	}
	else
	{
		if(filesystem_open(filename) == -1)
		{
			pcblock.file_struct[fd_index].flags = 0;
			memcpy(pcb_ptr, &pcblock, pcb_size);
			return -1;
		}
		pcblock.file_struct[fd_index].fpos = 0;
		pcblock.file_struct[fd_index].fops_ptr = &filesystem_fops;
		memcpy(pcb_ptr, &pcblock, pcb_size);
		return fd_index;
	}
 } 
 
 /*
 * int32_t close(int32_t fd)
 * 
 * INPUT: 
 * OUTPUT: 
 * RETURN: 
 */
 int32_t close (int32_t fd)
 {
	int i;
	// check for valid file descriptor
	if(fd > FD_MAX || fd < FD_MIN)
	{
		return -1;
	}
	
	if(pcblock.file_struct[fd].flags == 1)
	{
		pcblock.file_struct[fd].fpos = 0;
		pcblock.dentry[fd].file_type = NULL;
		pcblock.dentry[fd].inode_number = NULL;
		pcblock.file_struct[fd_index].fops_ptr = NULL;
				
		for(i = 0; i < B_32; i++)
		{
			pcblock.dentry[fd].file_name[i] = NULL;
		}
		for(i = 0; i < B_24; i++)
		{
			pcblock.dentry[fd].reserved[i] = NULL;
		}

		if(fd_rtc > 1 && fd == fd_rtc)
		{
			rtc_close(fd);
			pcblock.file_struct[fd].flags = 0;
			return 0;
		}
		else
		{
			filesystem_close(fd);
			pcblock.file_struct[fd].flags = 0;
			return 0;
		}
	}
	else
		return -1;
 }

 
 /*
 * find_pid
 * 
 * INPUT:
 * OUTPUT:
 * RETURN:
 */ 

 int find_pid()
 {
		uint32_t cr3;
		asm volatile ("mov %%CR3, %0": "=b"(cr3));
		if(cr3==(uint32_t)page_directory)
			return 0;
		if(cr3==(uint32_t)task1_page_directory)
			return 1;
		return -1;
 }


