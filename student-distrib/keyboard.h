/* keyboard.h - Defines used in interactions with the keyboard
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"
#include "types.h"
#include "i8259.h"
#include "terminal.h"

// These are a bunch of values for the keyboard when the button 
// pressed is not a printable character.
#define BACKSPACE			8
#define LEFT_SHIFT_PRESS	66
#define RIGHT_SHIFT_PRESS	67
#define CTRL_PRESS			65
#define ALT_PRESS			68
#define CAPS				69
#define F1					70
#define F2					71
#define F3					72
#define F4					73
#define F5					74
#define F6					75
#define F7					76
#define F8					77
#define F9					78
#define F10					79
#define F11					33
#define F12					34
#define NUM_LOCK			80
#define SCROLL_LOCK			81
#define HOME				81
#define UP					83
#define PAGE_UP				84
#define LEFT				85
#define RIGHT				87
#define END 				88
#define DOWN				89
#define PAGE_DOWN			90
#define INSERT				91
#define DELETE				92
#define CTRL_L				0xFF
#define CTRL_C				0xF8
#define CTRL_1				0xF1
#define CTRL_2				0xF2
#define CTRL_3				0xF3
#define CTRL_ALT			0xF9
#define TAB					0xFE
#define LEFT_SHIFT_R		0xAA
#define RIGHT_SHIFT_R		0xB6
#define CTRL_RELEASE		0x9D
#define ALT_RELEASE			0xBA
#define T1_SWITCH			0x9000
#define T2_SWITCH			0x9001
#define T3_SWITCH			0x9002
#define KB_DATA 			0x60
#define KEYBOARD_CMD_PORT   0x64
#define KEY_PRESS		0x80
extern unsigned char read_keyboard(); //Interrupt handler for the keyboard
unsigned char shift(unsigned char c); //For the shift key
#endif
