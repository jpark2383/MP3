#include "terminal.h"
#include "lib.h"
#include "x86_desc.h"
#include "rtc.h"

//declare arrays and variabls for text, history, counter, newline detector
unsigned char text_buf[BUF_MAX];
unsigned int counter;
int entered = 0;
//int newline = 0;

/*
 * set_cursor
 * move the cursor in the terminal
 * INPUT: x and y location
 * OUTPUT: NONE
 * RETURN: NONE
 * SIDE EFFECTS: Moves the cursor
 */

void set_cursor(int x, int y)
{
	uint32_t position = y*WIDTH + x;
	outb(LOW_CURSOR_PORT, VGA_CMD); //outb to the vga register
	outb((position & CURSOR_MASK), VGA_DATA);
	outb(HIGH_CURSOR_PORT, VGA_CMD);
	outb(((position>>SHIFT)&CURSOR_MASK), VGA_DATA);
	setx(x);
	sety(y);
}

/*
 * terminal_open
 * open the terminal and initialize buffer, enable irq, and move cursor to (0,0)
 * INPUT: NONE
 * OUTPUT: NONE
 * RETURN: 0 for success
 */

int32_t terminal_open(const uint8_t *filename)
{
	// use a loop to initialize the buffer to 0
	int i = 0;
	for(i = 0; i < BUF_MAX; i++)
	{
		text_buf[i] = NULL;
	}
	counter = 0; //initialize things
	set_cursor(0,0);
	enable_irq(PIC_1);
	return 0;
}

/*
 * terminal_close()
 * close the terminal and disable the irq
 * INPUT: NONE
 * OUTPUT: NONE
 * RETURN: -1 on success
 */

int32_t terminal_close(int32_t fd)
{
	disable_irq(PIC_1);
	return -1;
}

/*
 * keyboard_read
 * reads in the input from the keyboard and determines what to do with that
 * INPUT: NONE
 * OUTPUT: puts the character into the buffer
 * RETURN: NONE
 * SIDE EFFECTS: May print to the screen, move the cursor, delete characters, etc.
 */

void keyboard_read(unsigned char keystroke)
{
	//printf("keyboard_read called \n");
	int i, j;			//declare variables for counters
	int x = getx();
	int y = gety();
	set_cursor(x,y);
	uint8_t buf[BUF_MIN];
	//unsigned char keystroke = read_keyboard();
	//Condition: backspace
	if(keystroke == '\t')
	{
		printf("    ");
		set_cursor(x+4, y);
	}
	/* For another time
	else if(keystroke == UP)
	{
		if(counter < 80)
			set_cursor(0, y);
		else
			set_cursor(0, y - 1);
	}
	else if(keystroke == DOWN)
	{
		if(counter < 80)
			set_cursor(counter, y);
		else
			set_cursor(counter - 80, y + 1);
	}
	else if(keystroke == LEFT)
	{
		set_cursor(x - 1, y);
	}
	else if(keystroke == RIGHT)
	{
		if((x < counter) && (x < 79))
			set_cursor(x + 1, y);
		else if((x < counter) && (x == 79))
			set_cursor(0, y + 1);
	}*/
	else if (keystroke == CTRL_C)
	{
		send_eoi(PIC_8);
	}
	else if(keystroke == CTRL_L) //ctrl + l clears the screen
	{
		clear();
		set_cursor(0,0);
		printf("391OS> ");
	}
	else if((x >= WIDTH - 1) || (keystroke == '\n')) // Case when we reach bottom of screen
	{
		unsigned char text_hist[HEIGHT][WIDTH]; //array to hold previous display
		if(y >= HEIGHT - 1)
		{
			//save the display
			for(i = 0; i < HEIGHT; i++)
			{
				for(j = 0; j < WIDTH; j++)
					text_hist[i][j] = get_char(j, i);
			}
			clear();
			//shift the display up one position
			for(i = 1; i < HEIGHT; i++)
			{
				set_cursor(0,i - 1);
				for(j = 0; j < WIDTH; j++)
				{
					putc(text_hist[i][j]);
				}
			}
		}
		putc('\n'); // Write the new line
		if(keystroke == '\n') // This is when the key presses is actually \n
		{
			text_buf[counter] = keystroke;
			counter = 0;
			entered = 1;
			//newline = 1;
		}
		//set_cursor(0, y + 1);
	}
	else if(keystroke == BACKSPACE)
	{
		/*if(newline) // If line already empty
		{
			send_eoi(PIC_1);
			return;
		}*/
		for(i = 0; i < BUF_MIN; i++)
		{
			buf[i] = get_char(i, y);
		}
		if(!strncmp((int8_t*)buf, (int8_t*)"391OS> ", BUF_MIN) && (x == BUF_MIN))
		{
			send_eoi(PIC_1);
			return;
		}
		if(counter == 0)
		{
			send_eoi(PIC_1);
			return;
		}
		setx(--x); //These lines delete the character
		putc(' ');
		//setx(x);
		text_buf[--counter] = NULL; //Remove char from buffer
		set_cursor(x,y);
		//counter--;
	}
	// These are all the actual characters. Save to buffer and write to screen.
	else if((keystroke >= ' ') && (keystroke <= '~') && (counter < BUF_MAX - 1))
	{
		text_buf[counter] = keystroke;
		putc(keystroke);
		//set_cursor(x + 1, y);
		counter ++;
	}
	return;
}

/*
 * read_helper
 * writes keyboard data to the buffer to be written to the terminal
 * INPUT: fd: file discriptor
 *		*text: the buf with text
 *		length:length of the buf
 * OUTPUT: NONE
 * RETURN: display_terminal number
 */

int32_t read_helper(uint8_t *buf, int32_t length)
{
	//printf("read_helper called \n");
	//if((fd == 1) || (buf == NULL) || (length < 0))
	//	return -1;
	while(1)
	{
		//printf("got to line 205\n");
		/*if(newline == 1)
		{*/
			//printf("got to line 208\n");
		if(entered == 1)
		{
			int i = 0;
			int ret_val;
			//clear the buffer
			/*this might screw us up later*/
			//for(i = 0; i < length; i++)
				//buf[i] = NULL;
			//set new buffer values
			for(i = 0; (i < length) && (i < BUF_MAX); i++)
			{
				buf[i] = text_buf[i];
				//if newline is seen, break out of loop
				if(text_buf[i] == '\n')
				{
					ret_val = i;
					break;
				}
			}
			entered = 0;
			return ret_val;
		}
	}
}

/*
 * write_helper
 * write the buffer data to the terminal
 * INPUT: fd: file discriptor
 *		*text: the buf with text
 *		length:length of the buffer
 * OUTPUT: NONE
 * RETURN: display_terminal number
 */

 int32_t write_helper(const uint8_t* text, int32_t length)
 {
 	//printf("write_helper called \n");
 	int i, j, k;
 	int x = getx();
 	int y = gety();

 	unsigned char text_hist[HEIGHT][WIDTH]; // In case new line goes offscreen
 	//check for valid inputs
 	if (text == NULL || length < 0)
 		return -1;
 	for(k = 0; k < length; k++)
 	{
		/*if(y >= HEIGHT - 1)
		{
			printf("line 256\n");
			for(i = 0; i < HEIGHT; i++)
			{
				for(j = 0; j < WIDTH; j++)
					text_hist[i][j] = get_char(j, i);
			}
			clear();
			set_cursor(0,0);
			for(i = 1; i < HEIGHT; i++)
			{
				for(j = 0; j < WIDTH; j++)
				{
					set_cursor(0,i - 1);
					putc(text_hist[i][j]);
				}
			}
			set_cursor(0,y);
		}*/
		x = getx();

		y = gety();
		if(y >= HEIGHT - 1) //This is similar to the case in keyboard_read for page bottom
		{
			
			for(i = 0; i < HEIGHT - 1; i++)
			{
				for(j = 0; j < WIDTH; j++)
					text_hist[i][j] = get_char(j, i);
			}
			clear();
			//shift the screen
			for(i = 1; i < HEIGHT - 1; i++)
			{
				set_cursor(0,i - 1);
				for(j = 0; j < WIDTH; j++)
				{
					putc(text_hist[i][j]);
				}
			}
			set_cursor(0, HEIGHT - 2);
		}
 		if(text[k] == NULL)
 		{
 			//Do nothing
 		}
 		else if (text[k] == '\n')
 		{
 			set_cursor(0, gety()+1);
 		}
 		else
 		{
 			putc(text[k]); //Print the character
 		}
 		if((x == WIDTH - 1) && (text[k + 1] != '\n'))
 			putc('\n'); //Newline at end of line
 	}
 	set_cursor(getx(), gety());
 	return i+1;
 }

 /*
 * terminal_write
 * write the data in the buffer into the terminal
 * INPUT: fd: file discriptor; *text, the buf with text; length:length of the buf
 * OUTPUT: NONE
 * RETURN: display_terminal number
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t len)
{
	//printf("terminal write called\n");
	if (fd == 0) return -1;
	return write_helper((uint8_t*)buf, len);
}

/*
 * terminal_read
 * write the data from the keyboard into the buffer
 * INPUT: fd: file discriptor; *text, the buf with text; length:length of the buf
 * OUTPUT: NONE
 * RETURN: display_terminal number
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t len)
{
	if (fd == 1) return -1;
	return read_helper((uint8_t*)buf, len);
}
