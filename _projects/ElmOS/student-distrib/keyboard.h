#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"
#include "types.h"
#include "i8259.h"

#define keyboard_rw_port  0x60
#define keyboard_command_port   0x64

extern unsigned char read_translate_scancode();

#endif
