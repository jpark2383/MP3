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
uint32_t tasks[7] = {1,0,0,0,0,0,0}; //For the future, this will be 6


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
		printf("Cant exit from last shell");
		//while(1);
		return 0;
	}
	tasks[find_pid()] = 0;
	pc = pc-1;
	pcblock = *((pcb_t*)pcblock.prev_pcb);
	printf("pc: %d\n", pc);
	/*restore parents paging*/
	asm volatile ("mov %0, %%CR3":: "r"(pcblock.cr3));
	/*restore paresnts TSS kernel stack*/
	tss.esp0 = EIGHT_MB -4;	
	asm volatile("movl %0, %%esp	;"
				 "pushl %1			;"
				 ::"g"(pcblock.esp),"g"(status));
	asm volatile("movl %0, %%ebp"::"g"(pcblock.ebp));
	
	asm volatile("popl %eax");
	/*return 0*/
	asm volatile("movl $0, %eax");
	asm volatile("leave");
	asm volatile("ret");
	return 0;
	
}


/*
 * void parse_cmd(const uint8_t * command)
 * Parses the command string and loads the pcb with the command and argument
 * INPUT: const uint8_t command: command line string
 * OUTPUT: NONE
 * RETURN: NONE
 * SIDE EFFECTS: pcb is loaded with arg size, cmd, and arg
 */

void parse_cmd(const uint8_t * input)
{
	uint8_t input_length = 0;
	uint8_t cmd_size = 0;
	uint8_t arg_size = 0;
	uint8_t space_offset = 0;
	uint8_t i = 0;
	uint8_t switch_flag = 0;
	input_length = strlen((int8_t*)input);

	for(i = 0; i < CMD_LEN; i++){
		pcblock.cmd_name[i] = NULL;
		pcblock.arg_name[i] = NULL;
	}
	//printf("in parse command\n");
	//check if the first character is a space
	if (input[0] == ' ')
	{
		// count the amount of spaces before the first word
		for(i = 0; i < input_length; i++)
		{
			if(input[i] == ' ')
				space_offset++;
		}

		//read starting from the offset
		for(i = space_offset; i < input_length; i++)
		{	
			//if switch_flag = 0, we're reading the command
			if(switch_flag == 0 && input[i] != ' ')
			{
				pcblock.cmd_name[cmd_size] = input[i];
				cmd_size++;
			}
			//if switch_flag == 1, we're reading the argument
			if(switch_flag == 1 && input[i] != ' ')
			{
				pcblock.arg_name[arg_size] = input[i];
				arg_size++;
			}
			//check if next character is a space after the first string
			if(switch_flag == 0 && input[i + 1] == ' ')
			{
				switch_flag = 1;
			}
		}
		//load the size of the arg into the pcb
		pcblock.data_arg_size = arg_size;
		//printf("\nargd_size is set to %d\nThe command was set as %s\nThe argument was %s\n",pcblock.data_arg_size,pcblock.cmd_name,pcblock.arg_name);
	}
	//in the event that the first character is not a space
	else
	{
		//iterate through the string and seperate the cmd from the arg.
		for(i = 0; i < input_length; i++)
		{
			//if switch_flag = 0, we're reading the command
			if(switch_flag == 0 && input[i] != ' ')
			{
				pcblock.cmd_name[cmd_size] = input[i];
				cmd_size++;
			}
			//if switch_flag == 1, we're reading the argument
			if(switch_flag == 1 && input[i] != ' ')
			{
				pcblock.arg_name[arg_size] = input[i];
				arg_size++;
			}
			//check if next character is a space after the first string
			if(switch_flag == 0 && input[i + 1] == ' ')
			{
				switch_flag = 1;
			}
		}
		//load the size of the arg into the pcb
		pcblock.data_arg_size = arg_size;
		//printf("\nargd_size is set to %d\nThe command was set as %s\nThe argument was %s\n",pcblock.data_arg_size,pcblock.cmd_name,pcblock.arg_name);
	}
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
	asm volatile ("movl %%CR3, %0": "=b"(pcblock.cr3));
	int pd_addr;
	uint32_t eip = 0;
	uint32_t new_pid = 7;
	int i;
	for(i = 1; i < 7; i++)
	{
		if(tasks[i] == 0)
		{
			new_pid = i;
			tasks[i] = 1;
			break;
		}
	}
	if(command == NULL)
		return -1;
	if(new_pid == 7)
	{
		write(1, "6 programs already open.\n", 25);
		return -1;
	}


	parse_cmd(command);
	eip = loader(pcblock.cmd_name);
	if(eip == -1)
		return -1;
	/*check to see which task it is*/

	pcblock.file_struct[SDIN].flags =1;
	pcblock.file_struct[SDIN].fops_ptr = &terminal_fops;
	pcblock.file_struct[SDOUT].flags =1;
	pcblock.file_struct[SDOUT].fops_ptr = &terminal_fops;
	//mark all other file struct as unoccupied 
	for(i = 2; i <= MAX_FD; i++)
		pcblock.file_struct[i].flags= 0;
	//save paging
	/*flush the tlb*/
	asm volatile("movl %%cr3, %0" : "=r" (pd_addr));
	asm volatile("movl %0, %%cr3" : : "r" (pd_addr));

	//save the esp
	asm volatile("movl %%esp, %0"
     :"=r"(pcblock.esp)
     );
	asm volatile("movl %%ebp, %0" 
	: "=a"(pcblock.ebp)
	:
	: "cc" );
	uint32_t *pcbptr = (uint32_t *)(EIGHT_MB - STACK_EIGHTKB*(find_pid()) -START);
	memcpy(pcbptr, &pcblock, PCB_SIZE);
	pcblock.prev_pcb = (uint32_t)pcbptr;
	pcblock.pid = new_pid;
	/*check to see which program is running*/
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
		case 3:
			pcblock.cr3 = (uint32_t)task3_page_directory;
			break;
		case 4:
			pcblock.cr3 = (uint32_t)task4_page_directory;
			break;
		case 5:
			pcblock.cr3 = (uint32_t)task5_page_directory;
			break;
		case 6:
			pcblock.cr3 = (uint32_t)task6_page_directory;
			break;
		default:
			//write(1, "You done goofed", 15); //Error level over 9000
			return -1;
	}
	/*set up TSS for kernel mode*/
	tss.ss0 = KERNEL_DS;
	tss.esp0 = EIGHT_MB-KB_8*(pc-1) - 4;
	//counter++;
	/* inline assembly code to push the required variables to perform privilege switch*/
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
	int pid = find_pid();

	int index_temp;
	fd_index = FD_MIN;
	uint32_t *pcb_ptr = (uint32_t *)(EIGHT_MB - STACK_EIGHTKB*pid - START);
	//if filename is invalid, return -1
	if(filename == NULL)
	{
		return -1;
	}
	//Iterate through the array to find an unused fd.
	for(; fd_index <= FILE_ARRAY_SIZE; fd_index++)
	{
		if(pcblock.file_struct[fd_index].flags == 0)
		{
			index_temp = fd_index;
			break;
		}
	}
	/* if fd is full, return -1 */
	if (fd_index == FILE_ARRAY_SIZE)
		return -1;

	//set the fd_index and set the flag for the file descriptor.
	fd_index = index_temp;
	pcblock.file_struct[fd_index].flags = 1;

	//run filesystem_open to get the file type
	//filesystem_open(filename);
	if((strncmp((const int8_t*)filename, (const int8_t*)"" , 1)) != 0)
		read_dentry_by_name(filename, &pcblock.dentry[fd_index]);
	//if open fails, return -1
	if(filesystem_open(filename) == -1)
	{
		pcblock.file_struct[fd_index].flags = 0;
		memcpy(pcb_ptr, &pcblock, PCB_SIZE);
		return -1;
	}

	//check the file type, 0 = rtc.
	if(pcblock.dentry[fd_index].file_type == 0)
	{
		fd_rtc = fd_index;
		rtc_open(filename);
		pcblock.file_struct[fd_index].flags = 1;
		pcblock.file_struct[fd_rtc].fops_ptr = &rtc_fops;
		memcpy(pcb_ptr, &pcblock, PCB_SIZE);
		return fd_index;
	}
	else
	{
		pcblock.file_struct[fd_index].fpos = 0;
		pcblock.file_struct[fd_index].fops_ptr = &filesystem_fops;
		memcpy(pcb_ptr, &pcblock, PCB_SIZE);
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
		else if(cr3==(uint32_t)task1_page_directory)
			return 1;
		else if(cr3==(uint32_t)task2_page_directory)
			return 2;
		else if(cr3==(uint32_t)task3_page_directory)
			return 3;
		else if(cr3==(uint32_t)task4_page_directory)
			return 4;
		else if(cr3==(uint32_t)task5_page_directory)
			return 5;
		else if(cr3==(uint32_t)task6_page_directory)
			return 6;
		return -1;
 }
/*
 * getargs()
 * get the argument from terminal input
 * INPUT: buf -> copy into here; nbytes -> # of bytese to copy
 * OUTPUT: return 0 when successful, else return -1
 * RETURN: copy the argument into the buffer. 
 */
int32_t getargs (uint8_t* buf, int32_t nbytes)
{
	uint32_t i;
	if(buf == NULL || nbytes <0)
	{
		return -1;
	}
	for(i=0;i<1024;i++)
	{
		buf[i]=NULL;
	}
	if(pcblock.data_arg_size<nbytes)
		memcpy(buf,pcblock.arg_name,pcblock.data_arg_size);
	else
		memcpy(buf,pcblock.arg_name,nbytes);
		
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
