#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "types.h"
#include "keyboard.h"
#include "filesystem.h"

#define FILE_STRUCT_SIZE 	28
#define STRUCTS 			8
#define WIDTH 				80
#define HEIGHT 				25
#define LOW_CURSOR_PORT 	0x0F
#define HIGH_CURSOR_PORT 	0x0E
#define CURSOR_MASK			0xFF
#define VGA_CMD 			0x3D4
#define VGA_DATA			0x3D5
#define SHIFT				8
#define BUF_MAX				128
#define BUF_MIN				7

#define MEM_128MB		0x8000000   
#define MEM_32MB		0x02000000
#define MEM_6MB			0x600000
#define MEM_4MB         0x400000
#define MEM_4KB			0x1000	

#define V_MEM_ADDR	 	0xB8000
#define TERM1	 		MEM_6MB
#define TERM2 			MEM_6MB + MEM_4KB
#define TERM3 			MEM_6MB + MEM_4KB*2
#define T1_NUM			1
#define T2_NUM			2
#define T3_NUM			3

typedef struct terminal_t
{
	unsigned char t_linebuffer[BUF_MAX];
	int pos_x;
	int pos_y;
	uint32_t esp;
	uint32_t ebp;
	uint32_t kernel_esp;
	uint32_t cr3;
	uint32_t lb_ctr;
	file_struct_t file_struct[8];
} terminal_t;

terminal_t terminals[3];
int32_t term2_press;
int32_t term3_press;

extern void set_cursor(int x, int y); //Sets the cursor
extern void keyboard_read(unsigned char keystroke); //Parses whatever comes form the keyboard
extern int32_t terminal_open(const uint8_t* filename); //Opens the terminal
extern int32_t terminal_close(int32_t fd); //Closes the terminal
extern int32_t terminal_read(int32_t fd, void* buf, int32_t len); //Read from the terminal
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t len); //Write to the terminal
extern void multi_init(); // initialize the three terminals
//extern void start_terminal(int32_t t_num);
extern int32_t terminal_switch(int32_t terminal_num);//Switch terminals

int32_t read_helper(uint8_t *buf, int32_t length); //Slave to the read function
int32_t write_helper(const uint8_t* text, int32_t length); //More slaves!
int get_pid_from_cr3(uint32_t cr3);

#endif
