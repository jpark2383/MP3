/* keyboard.c - Functions to interact with the keyboard
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"

// handle numbers and lower case letters 
unsigned char key_codes[90] = {
0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 
	8,		//backspace
	'\t',		//tab
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 
	65,			//control
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
	66, 			//left shift
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 
	67,			//right shift
	'*',
	68, 			//alt
	' ',		//space bar
	69,			//caps lock
	70,			//59 - f1 key
	71, 72, 73, 74, 75, 76, 77, 78, 
	79,			//F10
	80,			//num lock
	81,			//scroll lock
	82,			//homekey
	83,			//up arrow
	84,			//page up
	'-',
	85,			//left arrow
	86,			
	87,			//right arrow
	'+',
	88,			//end key
	89,			//down arrow
	90,			//page down
	91,			//insert key
	92,			//delete key
	93,			
	94,
	95,
	33,			//F11 key
	34,			//F12 key
	35,			//undefined
	};

	/*
* read_keyboard
*   DESCRIPTION: Read scancode from keyboard's data port and translate it into char
*   INPUTS: None
*   OUTPUTS: None
*   RETURN VALUE: None
*   SIDE EFFECTS: Sends EOI to corresponding PIC
*/ 
void read_keyboard()
{
	unsigned char code;
	uint8_t scancode = inb(keyboard_rw_port);
	code = key_codes[scancode];
	if(scancode < valid_kb_press)
	{
		putc(code);
		send_eoi(1);
		return;
	}
	send_eoi(1);
}
