#ifndef _SYSTEMCALLS_H
#define _SYSTEMCALLS_H

#include "filesystem.h"
#include "types.h"
#include "lib.h"

#define pcb_size		745 // old value was 753.
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
//declaration of process control block.
//745
typedef struct __attribute__((packed)) pcb_t pcb_t;
struct pcb_t
{
	pcb_t *prev_pcb;
	uint32_t esp;
	uint32_t eip;
	uint32_t ebp;
	uint32_t cr3;
	uint32_t pid;
	file_struct_t file_struct[8]; //128
	
	dentry_t dentry[8]; // 512
	uint8_t cmd_name[CMD_LEN]; // 40
	uint8_t arg_name[CMD_LEN]; // 40
	uint8_t data_arg_size; // 1
};

pcb_t pcblock;
int fd_index;

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


