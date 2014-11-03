#include "keyboard.h"

// flags for shift, alt, caps, etc.
int shift_flag, alt_flag, caps_flag, ctrl_flag, l_shift, r_shift;
// handle numbers and lower case letters 
unsigned char key_codes[90] = {
0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 
	BACKSPACE,
	'\t',		//tab
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 
	CTRL_PRESS,			//control
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
	LEFT_SHIFT_PRESS, 			//left shift
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 
	RIGHT_SHIFT_PRESS, '*', ALT_PRESS,' ', CAPS,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,	//F1 - F10
	NUM_LOCK, SCROLL_LOCK, HOME, UP, PAGE_UP, '-', LEFT,
	86, RIGHT, '+', END, DOWN, PAGE_DOWN, INSERT,	DELETE,
	93, 94, 95, // Unknown
	F11, F12,	//F11-12 key
	35, 	//undefined
	};

unsigned char shift(unsigned char c)
{
	switch(c){
		case '`': return '~';
		case '1': return '!';
		case '2': return '@';
		case '3': return '#';
		case '4': return '$';
		case '5': return '%';
		case '6': return '^';
		case '7': return '&';
		case '8': return '*';
		case '9': return '(';
		case '0': return ')';
		case '-': return '_';
		case '=': return '+';
		case '[': return '{';
		case ']': return '}';
		case '\\': return '|';
		case ';': return ':';
		case '\'': return '"';
		case ',': return '<';
		case '.': return '>';
		case '/': return '?';
		default: return c;
	}
}

unsigned char read_keyboard()
{
	uint8_t scancode = inb(KB_DATA);
	unsigned char code = key_codes[scancode];
	unsigned char charval;
	if(scancode < KEY_PRESS)
	{
		//These are for setting the flags
		if(code == LEFT_SHIFT_PRESS)
		{
			l_shift = 1;
			shift_flag = 1;
			charval = NULL;
		}
		else if(code == RIGHT_SHIFT_PRESS)
		{
			r_shift = 1;
			shift_flag = 1;
			charval = NULL;
		}
		else if(code == ALT_PRESS)
		{
			alt_flag = 1;
			if(ctrl_flag)
				charval = CTRL_ALT;
			else charval = NULL;
		}
		else if(code == CTRL_PRESS)
		{
			ctrl_flag = 1;
			charval = NULL;
		}
		else if(code == CAPS)
		{
			if(caps_flag == 1)
				caps_flag = 0;
			else
				caps_flag = 1;
			charval = NULL;
		}
		else if((code >= '\'') && (code <= 'z'))
		{
			if(ctrl_flag == 1)
			{
				switch(code){
					case 'c': charval = CTRL_C;
					case 'l': charval = CTRL_L;
					default: break;
				}
			}
			else if((code <= 'z') && code >= 'a')
			{
				if(shift_flag == caps_flag) // lowercase letter
					charval = code;
				else
					charval = code - ('a' - 'A'); // uppercase letter
			}
			else if((code >= '\'' && code <= '@') || (code >= '[' && code <= '`'))
			{
				if(shift_flag)
					charval = shift(code);
				else charval = code;
			}
			else charval = NULL;
		}
		else
			charval = code;
	}
	else
	{
		if(scancode == LEFT_SHIFT_R)
			l_shift = 0;
		if(scancode == RIGHT_SHIFT_R)
			r_shift = 0;
		shift_flag = l_shift | r_shift;
		if(scancode == CTRL_RELEASE)
			ctrl_flag = 0;
		if(scancode == ALT_RELEASE)
			alt_flag = 0;
		charval = NULL;
	}
	keyboard_read(charval);
	send_eoi(PIC_1);
	return NULL;
}
