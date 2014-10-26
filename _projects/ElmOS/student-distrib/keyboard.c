#include "keyboard.h"

// handle numbers and lower case letters 
unsigned char key_codes[53] = {
0,		// blank
1,		// ESC
'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
14,		// BACKSPACE
15,		// TAB
'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 
28,		// ENTER
29,		// LEFT CONTROL
'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
42,		// LEFT SHIFT
'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'};

unsigned char read_translate_scancode(){
	uint8_t scancode = inb(keyboard_rw_port);
	return key_codes[scancode];
}

void keyboard_handler()
{
	unsigned char code;
	code = read_translate_scancode();
	putc(code);
	send_eoi(PIC_1);
}
