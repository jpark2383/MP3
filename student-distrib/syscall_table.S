.text

.global syscall_handler
.global halt
.global execute
.global read
.global write
.global open
.global close

syscall_handler:

	/* save all */
	pushl %eax
	push %es
	push %ds
	pushl %ebx
	pushl %ebp
	pushl %ecx
	pushl %edi
	pushl %edx
	pushl %esi
	
	/* what is this part */
	pushl %eax
	movw $0x18, %ax
	movw %ax, %ds
	popl %eax
	
	/* validity check on system call number */
	cmpl $0, %eax
	jbe  invalid_sysno
	cmpl $6, %eax
	jae	 invalid_sysno
	
	/* dispatch to jump table */
	jmp *system_call_table(, %eax, 4)
	

invalid_sysno:
	movl $-1, %eax
	jmp exit

exit:
	popl %esi
	popl %edx
	popl %edi
	popl %ecx
	popl %ebp
	popl %ebx
	popl %ds
	popl %es
	addl $4, %esp
	iret				/* interrupt return */


/* jump table for system calls */
system_call_table:
.long sys_empty, sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close

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




