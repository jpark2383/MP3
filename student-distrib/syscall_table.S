.text

.global syscall_handler
.global halt
.global execute
.global read
.global write
.global open
.global close
.global getargs
.global vidmap
.global set_handler
.global sigreturn

syscall_handler:

	/* save all */
	pushl %eax
	pushw %es
	pushw %ds
	pushw %fs
	pushw %gs
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	/* validity check on system call number */
	cmpl $0, %eax
	jbe  invalid_sysno
	cmpl $6, %eax
	jae	 invalid_sysno
	
	/* dispatch to jump table */
	jmp *system_call_table(, %eax, 4)

exit:
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	popw %gs
	popw %fs
	popw %ds
	popw %es
	addl $4, %esp
	iret				/* interrupt return */

invalid_sysno:
	movl $-1, %eax
	jmp exit

/* jump table for system calls */
system_call_table:
.long sys_empty, sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sys_sigreturn, 

sys_empty:

sys_halt:				/* status */
	pushl 	%ebx
	call	halt
	addl  	$4, %esp
	jmp		exit

sys_execute:			/* command */
	pushl 	%ebx
	call	execute
	addl  	$4, %esp
	jmp		exit

sys_read:				/* fd, buf, nbytes */
	pushl 	%edx
	pushl 	%ecx
	pushl 	%ebx
	call	read
	addl  	$12, %esp
	jmp		exit

sys_write:				/* fd, buf, nbytes */
	pushl 	%edx
	pushl 	%ecx
	pushl 	%ebx
	call	write
	addl  	$12, %esp
	jmp		exit

sys_open:				/* filename */
	pushl 	%ebx
	call	open
	addl  	$4, %esp
	jmp		exit

sys_close:				/* fd */
	pushl 	%ebx
	call	close
	addl  	$4, %esp
	jmp		exit
	
sys_getargs:				/* buf, nbytes */
	pushl 	%ecx
	pushl 	%ebx
	call	getargs
	addl  	$8, %esp
	jmp		exit

sys_vidmap:				/* screen_start */
	pushl 	%ebx
	call	vidmap
	addl  	$4, %esp
	jmp		exit

sys_set_handler:				/* signum, handler_address */
	pushl 	%ecx
	pushl 	%ebx
	call	set_handler
	addl  	$8, %esp
	jmp		exit
	
sys_sigreturn:				/* void */
	pushl 	%ebx
	call	sigreturn
	addl  	$4, %esp
	jmp		exit