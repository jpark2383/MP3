#ifndef _SYSTEMCALLS_H
#define _SYSTEMCALLS_H

#include "filesystem.h"
#include "types.h"
#include "lib.h"

#define pcb_size		753
#define MB_132			0x08400000
#define EIGHT_MB		0x800000
#define FOUR_MB			0x400000
#define ZERO_MB			0x0
#define STACK_EIGHTKB	0x2000
#define START			4
#define FILE_ARRAY_SIZE	8
#define video_mem		0xB8000
#define SDIN			0
#define SDOUT			1
#define T1_ARRAY_NUM	0
#define T2_ARRAY_NUM	1
#define T3_ARRAY_NUM	2
#define FD_MAX			7
#define FD_MIN			2

typedef struct __attribute__((packed)) pcb_t
{
	uint32_t *prev_pcb;
	uint32_t *espptr;
	uint32_t new_esp;
	uint32_t old_pc;
	uint32_t *ret_eip;
	uint32_t *ret_ebp;
	uint32_t *ret_pd;
	file_struct_t file_struct[8];
	
	dentry_t dentry[8];
	uint32_t signal;
	uint8_t actual[40];
	uint8_t data_arg[40];
	uint8_t data_arg_size;
} pcb_t;

pcb_t pcblock;
int fd_index;

int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
void pcb_clear();
int find_pid();

#endif

