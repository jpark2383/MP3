/* keyboard.h - Defines used in interactions with the keyboard
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"
#include "types.h"
#include "i8259.h"

#define keyboard_rw_port  0x60
#define keyboard_command_port   0x64
#define valid_kb_press		0x80
extern void read_keyboard();
#endif
