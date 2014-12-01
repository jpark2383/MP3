#include "systemcalls.h"
#include "rtc.h"
#include "terminal.h"
#include "filesystem.h"
#include "lib.h"
#include "x86_desc.h"
#include "pagefile.h"

//global variable for the rtc file descriptor.
int fd_rtc = 0;
//declartions of file ops for the rtc, terminal, and filesystem.
fops_t rtc_fops = {&rtc_open, &rtc_read, &rtc_write, &rtc_close};
fops_t terminal_fops = {&terminal_open, &terminal_read, &terminal_write, &terminal_close};
fops_t filesystem_fops = {&filesystem_open, &filesystem_read, &terminal_write, &filesystem_close};
uint32_t tasks[3] = {0,0,0}; //For the future, this will be 7


/*
 * int32_t halt (uint8_t status)
 * This system call will terminate the current process and return to the shell
 * INPUT: uint8_t status: ignore
 * OUTPUT: None
 * RETURN: Return 0
 */
int32_t halt (uint8_t status)
{
	//try to halt from shell, restart shell
	if(find_pid() == 1)
	{
		return 0;
	}
	pc = pc-1;
	pcblock = *(pcblock.prev_pcb);
	asm volatile ("mov %0, %%CR3":: "b"(task1_page_directory));
	tss.esp0 = EIGHT_MB -4;	
	asm volatile("movl %0, %%esp	;"
				 "pushl %1			;"
				 ::"g"(pcblock.esp),"g"(status));
	asm volatile("movl %0, %%ebp"::"g"(pcblock.ebp));
	
	asm volatile("popl %eax");
	asm volatile("leave");
	asm volatile("ret");
	return 0;
	
}

/*
 * int32_t execute (const uint8_t* command)
 * Starts a new process with the given command
 * INPUT: const uint8_t* command: string with the name of the program
 * OUTPUT: None
 * RETURN: 0 on success
 */
int32_t execute (const uint8_t* command)
{	
	int pd_addr;
	uint32_t eip = 0;
	uint32_t new_pid = 7;
	int i;
	if(command == NULL)
		return -1;
	eip = loader(command);
	if(eip == -1)
		return -1;
	/*switch(find_pid())
	{
		case 0: break; //wtf start over. you suck
	}	*/
	for(i = 0; i < 3; i++)
	{
		if(tasks[i] == 0)
		{
			new_pid = i;
			tasks[i] = 1;
			break;
		}
	}
	pcblock.file_struct[SDIN].flags =1;
	pcblock.file_struct[SDIN].fops_ptr = &terminal_fops;
	pcblock.file_struct[SDOUT].flags =1;
	pcblock.file_struct[SDOUT].fops_ptr = &terminal_fops;
	printf("%d", pc);
	//mark all other file struct as unoccupied 
	for(i = 2; i <= MAX_FD; i++)
		pcblock.file_struct[i].flags= 0;
	//save paging
	asm volatile("movl %%cr3, %0" : "=r" (pd_addr));
	asm volatile("movl %0, %%cr3" : : "r" (pd_addr));
	asm volatile ("movl %%CR3, %0": "=b"(pcblock.cr3));
	//put in page directory of new task
	//asm volatile ("movl %0, %%CR3":: "i"())
	//get the esp
	asm volatile("movl %%esp, %0"
     :"=r"(pcblock.esp)
     );
	asm volatile("movl %%ebp, %0" 
	: "=a"(pcblock.ebp)
	:
	: "cc" );
	//asm volatile("movl %0, %%esp" : : "r"(MB_132));
	pcb_t new_pcb = pcblock;
	new_pcb.prev_pcb = &pcblock;
	pcblock = new_pcb;
	pcblock.pid = new_pid;
	switch(new_pid)
	{
		case 0:
			pcblock.cr3 = (uint32_t)page_directory;
			break;
		case 1:
			pcblock.cr3 = (uint32_t)task1_page_directory;
			break;
		case 2:
			pcblock.cr3 = (uint32_t)task2_page_directory;
			break;
		default:
			write(1, "You done goofed", 15); //Error level over 9000
			return -1;
	}
	tss.ss0 = KERNEL_DS;
	tss.esp0 = EIGHT_MB-KB_8*(pc-1) - 4;
	//counter++;
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
 * int32_t read (int32_t fd, void* buf, int32_t nbytes)
 * System call for read. Uses a jump table to call the corresponding read function 
 * INPUT: int32_t fd: file descriptor
		  void* buf: buffer to be read to
		  int32_t nbytes: number of bytes to read
 * OUTPUT: Puts data into the buffer
 * RETURN: 0 on success, -1 otherwise
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
 * int32_t write (int32_t fd, void* buf, int32_t nbytes)
 * System call for write. Uses a jump table to call the corresponding read function 
 * INPUT: int32_t fd: file descriptor
		  void* buf: buffer to be written
		  int32_t nbytes: number of bytes to write
 * OUTPUT: Writes data from buffer
 * RETURN: 0 on success, -1 otherwise
 */
 
int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{
	if(buf == NULL || fd < 0 || fd > FD_MAX || pcblock.file_struct[fd].flags == 0 || nbytes < 0)
	{
		return -1;
	}
	return pcblock.file_struct[fd].fops_ptr->fops_write(fd, buf, nbytes);
}

/*
 * int32_t open(const uint8_t * filename)
 * System call for open
 * INPUT: const uint8_t* filename: file to be opened
 * OUTPUT: None
 * RETURN: File descriptor index of file opened, returns -1 on failure
 */
 int32_t open (const uint8_t* filename)
 {
	int pid = 0;
	pid = find_pid();
	int index_temp;
	fd_index = FD_MIN;
	uint32_t *pcb_ptr = (uint32_t *)(EIGHT_MB - STACK_EIGHTKB*pid - START);
	//if filename is invalid, return -1
	if(filename == NULL)
	{
		return -1;
	}
	//Iterate through the array to find an unused fd.
	for(; fd_index < FILE_ARRAY_SIZE; fd_index++)
	{
		if(pcblock.file_struct[fd_index].flags == 0)
		{
			index_temp = fd_index;
		}
	}

	//set the fd_index and set the flag for the file descriptor.
	fd_index = index_temp;
	pcblock.file_struct[fd_index].flags = 1;

	//run filesystem_open to get the file type
	filesystem_open(filename);
	//if open fails, return -1
	if(filesystem_open(filename) == -1)
	{
		pcblock.file_struct[fd_index].flags = 0;
		memcpy(pcb_ptr, &pcblock, pcb_size);
		return -1;
	}

	//check the file type, 0 = rtc.
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
		pcblock.file_struct[fd_index].fpos = 0;
		pcblock.file_struct[fd_index].fops_ptr = &filesystem_fops;
		memcpy(pcb_ptr, &pcblock, pcb_size);
		return fd_index;
	}
 } 
 
/*
 * int32_t close(const uint8_t * filename)
 * System call for close
 * INPUT: int32_t fd: file descriptor to be closed
 * OUTPUT: None
 * RETURN: 0 on success, -1 otherwise
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
 * int find_pid()
 * Finds the PID of the current process using CR3
 * INPUT: None
 * OUTPUT: The PID of the current process
 * RETURN: The PID of the current process
 */ 

 int find_pid()
 {
		uint32_t cr3;
		asm volatile ("movl %%CR3, %0": "=b"(cr3));
		if(cr3==(uint32_t)page_directory)
			return 0;
		if(cr3==(uint32_t)task1_page_directory)
			return 1;
		if(cr3==(uint32_t)task2_page_directory)
			return 2;
		return -1;
 }
/*
 * getargs()
 * get the argument of the command
 * INPUT: buf, the input buffer to be copied, nbytes, length you want to copy
 * OUTPUT: on succeed, return 0, else, return -1
 * RETURN: copy the command to the buf. 
 */
int32_t getargs (uint8_t* buf, int32_t nbytes)
{
/*
	uint32_t i;
	if(buf == NULL || nbytes <0)
	{
		return -1;
	}
	for(i=0;i<BYTES_4K;i++)
	{
		buf[i]=NULL;
	}
	if(pcba.argd_size<nbytes)
		memcpy(buf,pcblock.argd,pcblock.argd_size);
	else
		memcpy(buf,pcblock.argd,nbytes);
	*/	
	return 0;
}
/*
 * vidmap()
 * map the video memory into user space at a pre-set virtual address.
 * INPUT: the virtual start of the screen
 * OUTPUT: on succeed, return 0, else, return -1
 * RETURN: hide the video memory
 */
int32_t vidmap(uint8_t** screen_start)
{
	if(screen_start==ZERO_MB || screen_start==(uint8_t**)FOUR_MB || screen_start == NULL)
		return -1;
	*screen_start = (uint8_t *)video_mem;
	return 0;
}

int32_t set_handler (int32_t signum, void* handler)
{
	
	return 0;
}

int32_t sigreturn (void)
{
	
	return 0;
}


