.globl	keyboard_handler
.globl  rtc_handler

/*interrupt handler for keyboard, save all the registers and call read_keyboard*/
keyboard_handler:
_keyboard_handler:

		cli
		
		pushl %fs
		pushl %es
		pushl %ds
		pushl %eax
		pushl %ebx
		pushl %ebp
		pushl %ecx
		pushl %edx
		pushl %edi
		pushl %esi
		
	
		call read_keyboard
		
		popl %esi
		popl %edi
		popl %edx
		popl %ecx
		popl %ebp
		popl %ebx
		popl %eax
		popl %ds
		popl %es
		popl %fs

		sti
		
		iret
		
/*interrupt handler for rtc, save all registers and call rtc interrupt*/
rtc_handler:
_rtc_handler:

		cli
		
		pushl %fs
		pushl %es
		pushl %ds
		pushl %eax
		pushl %ebx
		pushl %ebp
		pushl %ecx
		pushl %edx
		pushl %edi
		pushl %esi
		
	
		call rtc_interrupt
		
		popl %esi
		popl %edi
		popl %edx
		popl %ecx
		popl %ebp
		popl %ebx
		popl %eax
		popl %ds
		popl %es
		popl %fs

		sti
		
		iret		

