#include "systemcalls.h"
#include "rtc.h"
#include "terminal.h"
#include "filesystem.h"
#include "lib.h"
#include "x86_desc.h"
#include "pagefile.h"

int fd_rtc = 0;
fops_t rtc_fops = {&rtc_open,&rtc_read,&rtc_write,&rtc_close};
fops_t terminal_fops = {&terminal_open,&terminal_read,&terminal_write,&terminal_close};
fops_t filesystem_fops = {&filesystem_open,&filesystem_read,&terminal_write,&filesystem_close};

/*
 * int32_t syscall_open(const uint8_t * filename)
 * 
 * INPUT: 
 * OUTPUT: 
 * RETURN: 
 */
 int32_t open (const uint8_t* filename)
 {
	int taken = 1;
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
			taken = 0;
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
 * int32_t syscall_close(int32_t fd)
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
 * read
 * 
 * INPUT:
 * OUTPUT: 
 * RETURN: 
 */
int32_t syscall_read(int32_t fd, void* buf, int32_t nbytes){
	if(fd > FD_MAX || fd < FD_MIN)				/* invalid file descriptor */
		return -1;
	if(pcblock.file_struct[fd].flags == 0)		/* current file descriptor is not being used*/
		return -1;

	return pcblock.file_struct[fd].fops_ptr->fops_read(fd, buf, nbytes);
}

 /*
 * write
 * System call write links it to the right handler depending on file type
 * INPUT: fd (file descriptor), buf (buffer to write), nbytes (# of bytes to write)
 * OUTPUT: runs write call
 * RETURN: number of bytes written
 */
int32_t syscall_write(int32_t fd, const void* buf, int32_t nbytes){
	if(fd > FD_MAX || fd < FD_MIN)				/* invalid file descriptor */
		return -1;
	if(pcblock.file_struct[fd].flags == 0)		/* current file descriptor is not being used*/
		return -1;
	
	return pcblock.file_struct[fd].fops_ptr->fops_write(fd, buf, nbytes);
}
 
 /*
 * find_pid
 * 
 * INPUT:
 * OUTPUT:
 * RETURN:
 */ 
 //This needs to be changed

 int find_pid()
 {
		uint32_t cr3;
		asm volatile ("mov %%CR3, %0": "=b"(cr3));
		if(cr3==(uint32_t)page_directory)
			return 0;
		if(cr3==(uint32_t)task1_page_directory)
			return 1;
		if(cr3==(uint32_t)task2_page_directory)
			return 2;
		if(cr3==(uint32_t)task3_page_directory)
			return 3;
		if(cr3==(uint32_t)task4_page_directory)
			return 4;
		if(cr3==(uint32_t)task5_page_directory)
			return 5;
		if(cr3==(uint32_t)task6_page_directory)
			return 6;
		return -1;
 }


