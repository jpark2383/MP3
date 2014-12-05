#include "keyboard.h"

// flags for shift, alt, caps, etc.
int shift_flag = 0, alt_flag, caps_flag = 0, ctrl_flag = 0, l_shift = 0, r_shift = 0;
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

/*
 * shift(unsigned char c)
 * applies shift key to characters
 * INPUT: unsigned char c: character to be shifted
 * RETURN: shifted character
 * SIDE EFFECTS: None
 */
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

/*
 * read_keyboard
 * interrupt handler for the keyboard
 * INPUT: none
 * OUTPUT: NONE
 * RETURN: NULL to indicate success
 * SIDE EFFECTS: Handles keyboard, may write to terminal, and sends EOI
 */
unsigned char read_keyboard()
{
	uint8_t scancode = inb(KB_DATA);
	unsigned char code = key_codes[scancode];
	unsigned char charval;
	if(scancode < KEY_PRESS)
	{
		//These are for setting the flags
		if(code == LEFT_SHIFT_PRESS) //On left shift press
		{
			l_shift = 1;
			shift_flag = 1;
			charval = NULL;
		}
		else if(code == RIGHT_SHIFT_PRESS) //On right shift press
		{
			r_shift = 1;
			shift_flag = 1;
			charval = NULL;
		}
		else if(code == ALT_PRESS) //On alt press
		{
			alt_flag = 1;
			if(ctrl_flag)
				charval = CTRL_ALT;
			else charval = NULL;
		}
		else if(code == CTRL_PRESS) //On control press
		{
			ctrl_flag = 1;
			charval = NULL;
		}
		else if(code == CAPS) //On caps lock press
		{
			if(caps_flag == 1)
				caps_flag = 0;
			else
				caps_flag = 1;
			charval = NULL;
		}
		
		/* Pseudo code for 
		else if(alt_flag == 1 && (code >= 'F1' || code <= 'F3'))
		{
			if(code == 'F1')
				charval = T1_SWITCH;
				break;
			else if(code == 'F2')
				charval = T2_SWITCH;
				break;
			else if(code == 'F3')
				charval = T3_SWITCH
				break;
		}*/

		else if((code >= '\'') && (code <= 'z')) // If it is a character
		{
			if(ctrl_flag == 1) // control is pressed
			{
				switch(code){
					case 'c': 
						charval = CTRL_C; //interrupt
						break;
					case 'l': 
						charval = CTRL_L; //Special CTRL_L value
						break;
					/*case '1': charval = CTRL_1; //first terminal
					case '2': charval = CTRL_2; //second terminal
					case '3': charval = CTRL_3; //thrid terminal*/
					default: break;
				}
			}
			else if((code <= 'z') && (code >= 'a')) //If it is a letter
			{
				if(shift_flag == caps_flag) // lowercase letter
					charval = code;
				else
					charval = code - ('a' - 'A'); // uppercase letter
			}
			//If not a letter
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
		if(scancode == LEFT_SHIFT_R) // On left shift release
			l_shift = 0;
		else if(scancode == RIGHT_SHIFT_R) // On right shift release
			r_shift = 0;
		else if(scancode == CTRL_RELEASE) // On ctrl release
			ctrl_flag = 0;
		else if(scancode == ALT_RELEASE) // On alt release
			alt_flag = 0;
		shift_flag = l_shift || r_shift;
		charval = NULL;
	}
	keyboard_read(charval);
	send_eoi(PIC_1);
	return NULL;
}
