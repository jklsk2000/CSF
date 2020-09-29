/* Assembly language implementation of hexdump functions */

	.section .rodata

	.section .text


/*
 * Read up to 16 characters from stdin and copy it over to %rsi
 * Returns the number of characters read.
 * 
 * Register Use:
 *  %rdi: parameter with empty char array
 *		later used for fildes argument
 *	%rsi: for *buf argument (from %rdi)
 * 	%rdx: for count (size) argument
 * 	%rax: for read (0) syscall, later stores
 *		the number of chars read
 */
	.globl hex_read
hex_read:
	subq $8, %rsp

	/* move addr of the arr in %rsi */
	/* fildes set to 0 for STDIN in %rdi */
	/* read up to 16 chars set by %rdx */
	/* set %rax to 0 for sys_read */

	movq %rdi, %rsi
	movq $0, %rdi
	movq $16, %rdx
	movq $0, %rax
	syscall

	addq $8, %rsp
	ret

/*___________________________________________________________________*/

/*
 * Writes the nul-terminated char array to stdout.
 *
 * Register Use:
 *  %rdi: parameter with const char array
 *		later used for fildes argument
 *	%rsi: for *buf argument (from %rdi)
 * 	%rdx: for count (size) argument
 * 	%r8: temporary holder for %rdi
 * 	%r10: counter used to find size of array
 * 	%rax: for write (1) syscall
 */
	.globl hex_write_string
hex_write_string:
	subq $8, %rsp

	/* loop to calculate the length of string */
	movq $0, %r10
	movq %rdi, %r8

.LstrLenLoop:
	cmpb $0, (%r8)
	jz .LstrLenDone
	inc %r10
	inc %r8
	jmp .LstrLenLoop

.LstrLenDone:
	/* move to the size of the array to %rdx argument */
	movq %r10, %rdx

	/* move addr of the arr to %rsi */
	/* set fildes to STDOUT (1) to %rdi */
	/* set %rax to 1 for sys_write */
	movq %rdi, %rsi
	movq $1, %rdi
	movq $1, %rax
	syscall

	addq $8, %rsp
	ret

/*___________________________________________________________________*/

/*
 * Formats a long value in hex digits and put into a char array
 *
 * Register Use:
 *  %rdi: value of the character in long to be formatted
 * 	%rsi: char array that will hold the hex digits
 * 	%r10: loop counter
 * 	%r11: temporary storage for hex digits
 */	
	.globl hex_format_offset
hex_format_offset:
	subq $8, %rsp

	/* set the last element to \0 */
	movb $0, 8(%rsi)
	/* construct a loop of 8 counts */
	movq $7, %r10
.LoffsetLoop:
	/* end loop if index is less than 0 */
	cmpq $0, %r10
	jl .LhexLoopDone

	/* move byteval to temporary storage */
	/* take the 4 least sig bits */
	movq %rdi, %r11
	andq $15, %r11

	/* 2 cases: */
	/* 1) less than 10 -> numbers */
	/* 2) greater than or equal 10 -> alphabets */
	cmpq $10, %r11
	jge .LoffsetAlphabet

	/* adjust %r11 accordingly to the ascii values */
	addq $48, %r11
	jmp .LoffsetCont

.LoffsetAlphabet:
	addq $87, %r11

	/* input the new hex into the char array */
	/* shift right to the next 4 bits */
	/* decrease the loop counter and continue loop */
.LoffsetCont:
	movb %r11b, (%rsi, %r10)
	shrq $4, %rdi
	decq %r10
	jmp .LoffsetLoop

.LoffsetDone:
	addq $8, %rsp
	ret

/*___________________________________________________________________*/

/*
 * Formats each long value to 2 digit hex value
 * and inputs it into a char array. 
 * 
 * Register Use:
 *  %rdi: value of the character in long to be formatted
 * 	%rsi: char array that will hold the hex digits
 * 	%r10: loop counter
 * 	%r11: temporary storage for hex digits
 */	
	.globl hex_format_byte_as_hex
hex_format_byte_as_hex:
	subq $8, %rsp

	
	/* set the last element to \0 */
	movb $0, 2(%rsi)
	/* construct a loop of 2 counts */
	movq $1, %r10
.LhexLoop:
	/* end loop if index is less than 0 */
	cmpq $0, %r10
	jl .LhexLoopDone

	/* move byteval to temporary storage */
	/* take the 4 least sig bits */ 
	movq %rdi, %r11
	andq $15, %r11

	/* 2 cases: */
	/* 1) less than 10 -> numbers */
	/* 2) greater than or equal 10 -> alphabets */
	cmpq $10, %r11
	jge .LalphabetCase

	/* adjust %r11 accordingly to the ascii values */
	addq $48, %r11
	jmp .LhexLoopCont

.LalphabetCase:
	addq $87, %r11

	/* input the new hex into the char array */
	/* shift right to the next 4 bits */
	/* decrease the loop counter and continue loop */
.LhexLoopCont:
	movb %r11b, (%rsi, %r10)
	shrq $4, %rdi
	decq %r10
	jmp .LhexLoop

.LhexLoopDone:
	addq $8, %rsp
	ret

/*___________________________________________________________________*/

/*
 * Converts non-printable characters of the ASCII table to a dot.
 * Keeps other characters unmodified.
 *
 * Register Use:
 *  %rdi: parameter with decimal value of a character
 * 	%rax: newly printable decimal value of a character
 */
	.globl hex_to_printable
hex_to_printable:
	subq $8, %rsp

	/* if %rdi is less than or equal to 31 */
	/* or if %rdi is greater than or equal to 127 */
	/* jump to .LnonPrintable */
	cmpq $31, %rdi
	jle .LnonPrintable
	cmpq $127, %rdi
	jge .LnonPrintable

	/* if %rdi is in between, don't modify it */
	/* pass to %rax and jump to .L...done */
	movq %rdi, %rax
	jmp .LprintableDone

/* change non-printable to 46 (which is a dot) */
.LnonPrintable:
	movq $46, %rax

.LprintableDone:
	addq $8, %rsp
	ret

/* vim:ft=gas:
 */
