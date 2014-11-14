#include "systemcalls.h"
#include "rtc.h"
#include "terminal.h"
#include "filesystem.h"
#include "lib.h"
#include "x86_desc.h"

int fd_rtc = 0;
fops_t rtc_fops = {&rtc_open,&rtc_read,&rtc_write,&rtc_close};
fops_t terminal_fops = {&terminal_open,&terminal_read,&terminal_write,&terminal_close};
fops_t filesystem_fops = {&filesystem_open,&filesystem_read,&terminal_write,&filesystem_close};

/*
 * int32_t open(const uint8_t * filename)
 * find a new unused fd, and mark it used. and open it.  
 * INPUT: filename, the name of the file you want to open.
 * OUTPUT: on succeed, return fd number, else, return -1
 * RETURN: open a new fd
 */
 
 int32_t open (const uint8_t* filename)
 {
	int full = 1;
	int pid = 0;
	pid = find_pid();
	uint32_t *pcb_ptr = (uint32_t *)(EIGHT_MB - STACK_EIGHTKB*pid - START);
	//if filename is invalid, return -1
	if(filename == NULL)
	{
		return -1;
	}
	//check each file descriptor for one that hasn't been used.
	for(fd_index = FD_MIN; fd_index < FILE_ARRAY_SIZE; fd_index++)
	{
		if(pcblock.file_struct[fd_index].flags == 0)
		{
			full = 0;
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
 * close the current fd
 * INPUT: fd number to be closed
 * OUTPUT: on succeed, return 0, else, return -1
 * RETURN: close a fd. 
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
		for(i = 0; i < B_32; i++)
		{
			pcblock.dentry[fd].file_name[i] = NULL;
		}
		for(i = 0; i < B_24; i++)
		{
			pcblock.dentry[fd].reserved[i] = NULL;
		}
		pcblock.dentry[fd].file_type = NULL;
		pcblock.dentry[fd].inode_number = NULL;
		pcblock.file_struct[fd_index].fops_ptr = NULL;
		
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
 * get_pid
 * get the pid number of the current process
 * INPUT: NONE
 * OUTPUT: wpid number
 * RETURN: NONE
 */ 
 //This needs to be changed
 int find_pid()
 {
		uint32_t cr3;
		asm volatile ("mov %%CR3, %0": "=b"(cr3));
		if(cr3==(uint32_t)page_directory)
			return 0;
		if(cr3==(uint32_t)page_directory_task1)
			return 1;
		if(cr3==(uint32_t)page_directory_task2)
			return 2;
		if(cr3==(uint32_t)page_directory_task3)
			return 3;
		if(cr3==(uint32_t)page_directory_task4)
			return 4;
		if(cr3==(uint32_t)page_directory_task5)
			return 5;
		if(cr3==(uint32_t)page_directory_task6)
			return 6;
		return -1;
 }
 