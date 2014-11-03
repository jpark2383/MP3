#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "types.h"
#include "keyboard.h"

#define FILE_STRUCT_SIZE 	28
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

extern void set_cursor(int x, int y);
extern void keyboard_read();
extern int32_t terminal_open(const uint8_t* filename);
extern int32_t terminal_close(int32_t fd);
extern int32_t terminal_read(int32_t fd, uint8_t *buf, uint32_t len);
extern int32_t terminal_write(int32_t fd, const uint8_t *buf, uint32_t len);

int32_t read_helper(int32_t fd, uint8_t *buf, uint32_t length);
int32_t write_helper(int32_t fd, const uint8_t *text, uint32_t length);

#endif
