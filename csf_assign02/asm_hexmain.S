/* Hexdump main function, assembly language version */
	.section .rodata

sColon: .string ":"
sSpace: .string " "

sFmt: .string "%c\n"

	.section .data

hexdump: .zero 78
databuf: .zero 17

	.section .text

/*------------------------------------------------------------------------------------*/

/*
 * Register Use:
 *	%r13: ptr to hexdump arr addr
 *	%r14: index of hexdump arr
 * 	%rbp: base pointer to store offset arr
 *	%rdi: first argument - index of hexdump arr
 *	%rsi: second arugment - offset position
 *	%rcx: loop counter
 * 	%r8: temporary variable to store offset arr elts
 *	%rax: return value storing updated index
 */
	.globl offset
offset:
	pushq %r12				/* save callee saved registers */
	pushq %r13
	pushq %r14
	pushq %r15
	pushq %rbx
	pushq %rbp
	subq $24, %rsp
	movq %rsp, %rbp

	movq %rdi, %r14			/* move hexdump arr index to %rdx */

	movq %rsi, %rdi 		/* move offset value as first arg */
	movq %rbp, %rsi 		/* pass the array as second arg */
	call hex_format_offset	/* call hex_format_offset function */

	movq $0, %rcx 			/* use %rcx as loop counter */
	movq $hexdump, %r13		/* move addr of hexdump arr to %r13 */

.LoffsetLoop:
	cmpq $8, %rcx			/* loop of count 8 */
	jge .LoffsetDone		/* if counter is >= 8, stop */

	movb (%rbp, %rcx), %r8b	/* store one offset elt temporarily */
	movb %r8b, (%r13, %r14)	/* transfer offset elt to hexdump arr */
	incq %r14				/* increment hexdump arr index */
	incq %rcx				/* increment loop counter */
	jmp .LoffsetLoop		/* continue loop */

.LoffsetDone:
	movb $58, %r8b		
	movb %r8b, (%r13, %r14)	/* add a ":" at the end of offset */
	incq %r14				/* increment hexdump arr index */

	movq %r14, %rax			/* move hexdump arr index to %rax to return */
	addq $24, %rsp			/* realign stack */
	popq %rbp
	popq %rbx
	popq %r15			
	popq %r14
	popq %r13
	popq %r12				/* restore callee saved registers */
	ret

/*------------------------------------------------------------------------------------*/

/*
 * Register Use:
 * 	%r12: ptr to databuf arr addr
 *	%r13: ptr to hexdump arr addr
 *	%r14: index of hexdump arr
 *	%rbx: length of databuf arr
 * 	%r15: loop counter
 * 	%r8: signal for last line
 * 	%r9: temporary storage for hex digits
 */
	.globl hexString
hexString:
	pushq %r12				/* save callee saved registers */
	pushq %r13
	pushq %r14
	pushq %r15
	pushq %rbx
	pushq %rbp
	subq $24, %rsp			/* stack alignment */
	movq %rsp, %rbp

	movq $databuf, %r12		/* move addr of databuf arr to %r12 */
	movq $hexdump, %r13		/* move addr of hexdump arr to %r13 */
	movq %rdi, %r14			/* store hexdump index in %r14 */
	movq %rsi, %rbx			/* store databuf arr len in %rbx */

	movq $0, %r15			/* %r15 will be the loop counter */
	movq $0, %r8			/* %r8 will indicate if this is the last line */
	cmpq $16, %rbx			/* compare the length of databuf arr to 16 */
	je .LhexLoop			/* if = 16, then it's not the last line */
	movq $1, %r8			/* if < 16, then change %r8 to indicate it */

.LhexLoop:
	cmpq $16, %r15			/* check if %r15 has reached 16 */
	jge .LhexDone			/* if %r15 >= 16 then exit loop */

	movb $32, %r9b 			/* store ascii value for space in %r9 */
	movb %r9b, (%r13, %r14)	/* input space in hexdump arr */
	incq %r14				/* increment hexdump arr index */

	cmpq $0, %r8			/* if !last, jmp to .LhexCall */
	je .LhexCall
	cmpq %r15, %rbx			/* if last & no more chars, fill w/ spaces */
	jle .LhexFill

.LhexCall:
	movb (%r12, %r15), %dil	/* move an elt of databuf to convert */
	movq %rbp, %rsi			/* pass the arr ptr to store hex vals */
	call hex_format_byte_as_hex

	movb 0(%rbp), %r9b		/* copy first hex digit to temp */
	movb %r9b, (%r13, %r14)	/* transfer to hexdump arr */
	incq %r14				/* increment hexdump arr index */
	movb 1(%rbp), %r9b		/* copy second hex digit to temp */
	movb %r9b, (%r13, %r14)	/* transfer to hexdump arr */
	incq %r14				/* increment hexdump arr index */
	jmp .LhexNext

.LhexFill:
	movb $32, %r9b			/* put ascii val for space to temp */
	movb %r9b, (%r13, %r14)	/* fill 2 spots in hexdump w/ space */
	incq %r14
	movb %r9b, (%r13, %r14)
	incq %r14

.LhexNext:
	incq %r15				/* increment loop counter */
	jmp .LhexLoop			/* continue loop */

.LhexDone:
	movq %r14, %rax			/* move updated index to %rax to return */

	addq $24, %rsp			/* realign stack */
	popq %rbp
	popq %rbx
	popq %r15
	popq %r14
	popq %r13
	popq %r12				/* restore callee saved registers */
	ret

/*------------------------------------------------------------------------------------*/

/*
 * Register Use:
 * 	%r12: ptr to databuf arr addr
 *	%r13: ptr to hexdump arr addr
 *	%r14: index of hexdump arr
 *	%rbx: length of databuf arr
 * 	%r15: loop counter
 * 	%r9: temporary storage
 *	%rax: temporary storage for printables & index
 */
	.globl printable
printable:
	pushq %r12				/* save callee saved registers */
	pushq %r13
	pushq %r14
	pushq %r15
	pushq %rbx
	pushq %rbp
	subq $8, %rsp			/* stack alignment */

	movq $databuf, %r12		/* move addr of databuf arr to %r12 */
	movq $hexdump, %r13		/* move addr of hexdump arr to %r13 */
	movq %rdi, %r14			/* store hexdump index in %r14 */
	movq %rsi, %rbx			/* store databuf arr len in %rbx */

	movb $32, %r9b			/* put ascii val for space to temp */
	movb %r9b, (%r13, %r14)	/* fill 2 spots in hexdump w/ space */
	incq %r14
	movb %r9b, (%r13, %r14)
	incq %r14

	movq $0, %r15			/* designate %r15 to be loop counter */

.LprintLoop:
	cmpq $16, %r15			/* if < 16, continue loop */
	jge .LprintDone			/* if >= 16, exit loop */
	cmpq %r15, %rbx			/* compare databuf arr len to loop count */
	jle .LprintDone 		/* if arr len is <= loop count, break */

	movb (%r12, %r15), %dil	/* set an elt of databuf arr as first arg */
	call hex_to_printable

	movb %al, (%r13, %r14)	/* store the updated printable char in hexdump */
	incq %r14				/* increment hexdump arr index */
	incq %r15				/* increment loop counter */
	jmp .LprintLoop			/* continue loop */

.LprintDone:
	movb $10, (%r13, %r14)	/* end the hexdump with a new line */
	incq %r14				/* increment hexdump arr index */
	movb $0, (%r13, %r14)	/* and a null terminator */
	incq %r14				/* increment hexdump arr index */

	movq %r14, %rax 		/* move updated index to %rax to return */

	addq $8, %rsp			/* realign stack */
	popq %rbp
	popq %rbx
	popq %r15
	popq %r14
	popq %r13
	popq %r12				/* restore callee saved registers */
	ret

/*------------------------------------------------------------------------------------*/

/*
 * Register Use:
 * 	%r10: pointer to the addr of databuf arr
 *	%r11: pointer to the addr of hexdump arr
 *	%r12: length of databuf arr
 *	%r13: index of hexdump arr
 *	%r14: value of offset position
 *	%rdi: first argument
 *	%rsi: second argument
 *	%rax: return value
 */
	.globl main
main:
	subq $8, %rsp		/* stack alignment */

	movq $databuf, %r12	/* move addr of databuf to %r12 */
	movq $hexdump, %r13 /* move addr of hexdump to %r13 */
	movq $0, %r14		/* index of hexdump arr to %r14 */
	movq $0, %r15		/* value of position offset to %r15 */
	movq $0, %rbx		/* number of databuf arr elts to %rbx */

	movq %r12, %rdi		/* set databuf arr as first arg */
	call hex_read		/* call hex_read function */
	movq %rax, %rbx 	/* move hex read arr length to %rbx */

.Ltop:
	cmpq $0, %rbx			/* if hex read length is smaller than 1, */
	jle .Ldone				/* end the loop */
	movb $0, (%r12, %rbx)	/* set last arr elt to nul terminator */
	


	movq %r14, %rdi		/* move hexdump arr index as first arg */
	movq %r15, %rsi 	/* move position offset as second arg */
	call offset
	movq %rax, %r14		/* update hexdump arr index w/ return value */
	addq $16, %r15		/* increment the offset by 16 */

	/* DEV */

	movq %r14, %rdi		/* move hexdump arr index as first arg */
	movq %rbx, %rsi		/* move length of databuf arr as second arg */
	call hexString
	movq %rax, %r14		/* update hexdump arr index w/ return value */

	movq %r14, %rdi		/* move hexdump arr index as first arg */
	movq %rbx, %rsi		/* move length of databuf arr as second arg */
	call printable
	movq %rax, %r14		/* update hexdump arr index w/ return value */

	/* DEV */

	movq %r13, %rdi		/* set hexdump arr as first arg */
	call hex_write_string

	movq %r12, %rdi		/* set databuf arr as first arg */
	call hex_read		
	movq %rax, %rbx 	/* move databuf arr length to %rbx */

	movq $0, %r14		/* reset index to 0 for new hexdump line */
	jmp .Ltop			/* continue loop */

.Ldone:
	addq $8, %rsp
	ret

/* vim:ft=gas:
 */
