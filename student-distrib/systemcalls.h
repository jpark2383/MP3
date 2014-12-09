#ifndef _SYSTEMCALLS_H
#define _SYSTEMCALLS_H

#include "filesystem.h"
#include "types.h"
#include "lib.h"

#define PID1			1
#define PID2			2
#define PID3			3
#define PID4			4
#define PID5			5
#define PID6			6
#define PCB_SIZE		745
#define MB_132			0x08400000
#define KB_8			0x2000
#define EIGHT_MB		0x800000
#define FOUR_MB			0x400000
#define ZERO_MB			0x0
#define STACK_EIGHTKB	0x2000
#define START			4
#define FILE_ARRAY_SIZE	8
#define video_mem		0xB8000
#define T1_ARRAY_NUM	0
#define T2_ARRAY_NUM	1
#define T3_ARRAY_NUM	2
#define FD_MAX			7
#define FD_MIN			2
#define SDIN			0
#define SDOUT			1
#define MAX_FD			7
#define CMD_LEN			40
#define MAX_TASKS		7
#define T_SHELL_MAX		3
#define BUFF_MAX		1024
#define T_1				1
#define T_2				2
#define T_3				3
#define FILE_MAX		8
#define FILE_MIN		2
#define T_6				6

//declaration of process control block.
typedef struct __attribute__((packed)) pcb_t pcb_t;
struct pcb_t
{
	uint32_t prev_pcb; // Pointer to previous pcb
	uint32_t esp; //The following are saved registers
	uint32_t eip;
	uint32_t ebp;
	uint32_t cr3;
	uint32_t parent_pid; //Pid of parent process
	file_struct_t file_struct[FILE_ARRAY_SIZE]; //Array of fops
	
	dentry_t dentry[FILE_ARRAY_SIZE]; //data entries
	uint8_t cmd_name[CMD_LEN]; //The following are for execution
	uint8_t arg_name[CMD_LEN];
	uint8_t data_arg_size;
};

pcb_t pcblock;
int fd_index;
uint32_t new_pid;

/* function declaratios */
int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);
void parse_cmd(const uint8_t * input);
void pcb_clear();
extern int find_pid();

#endif


