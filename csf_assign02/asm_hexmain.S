/* Hexdump main function, assembly language version */
	.section .rodata

sColon: .string ":"
sSpace: .string " "

	.section .data

hexdump: .zero 78
databuf: .zero 17
hexdigit: .zero 2

	.section .text

/*------------------------------------------------------------------------------------*/

/*
 * Register Use:
 *	%r12: callee saved register
 *	%r13: callee saved register
 *	%r14: callee saved register
 * 	%rbp: base pointer
 *	%rdi: first argument - index of hexdump arr
 *	%rsi: second arugment - offset position
 *	%rdx: store for hexdump arr index
 *	%rcx: loop counter
 *	%r11: pointer to the addr to hexdump arr
 * 	%r8: temporary variable
 *	%rax: return value storing updated index
 */
	.globl offset
offset:
	pushq %r12				/* save callee saved registers */
	pushq %r13
	pushq %r14
	pushq %rbp
	subq $16, %rsp
	movq %rsp, %rbp

	movq %rdi, %rdx			/* move hexdump arr index to %rdx */
	movq %rsi, %rdi 		/* move offset value as first arg */
	movq %rbp, %rsi 		/* pass the array as second arg */
	call hex_format_offset	/* call hex_format_offset function */

	movq $0, %rcx 			/* use %rcx as loop counter */
	movq $hexdump, %r11		/* move addr of hexdump arr to %r11 */

.LoffsetLoop:
	cmpq $8, %rcx			/* loop of count 8 */
	jge .LoffsetDone		/* if counter is >= 8, stop */

	movb (%rbp, %rcx), %r8b	/* store one offset elt temporarily */
	movb %r8b, (%r11, %rdx)	/* transfer offset elt to hexdump arr */

	incq %rdx				/* increment hexdump arr index */
	incq %rcx				/* increment loop counter */
	jmp .LoffsetLoop		/* continue loop */

.LoffsetDone:
	movb $58, %r8b		
	movb %r8b, (%r11, %rdx)	/* add a ":" at the end of offset */
	incq %rdx				/* increment hexdump arr index */

	movq %rdx, %rax			/* move hexdump arr index to %rax to return */
	addq $16, %rsp			/* realign stack */
	popq %rbp			
	popq %r14
	popq %r13
	popq %r12				/* restore callee saved registers */
	ret

/*------------------------------------------------------------------------------------*/
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

	movq $databuf, %r10	/* move addr of databuf to %r10 */
	movq $hexdump, %r11 /* move addr of hexdump to %r11 */
	movq $0, %r12		/* long value returned from databuf array */
	movq $0, %r13		/* index of the hexdump array */
	movq $0, %r14		/* value of the position offset */

	movq %r10, %rdi		/* set databuf arr as first arg */
	call hex_read		/* call hex_read function */
	movq %rax, %r12 	/* move hex read arr length to %r12 */

.Ltop:
	cmpq $0, %r12			/* if hex read length is smaller than 1, */
	jle .Ldone				/* end the loop */
	movb $0, (%r10, %r12)	/* set last arr elt to nul terminator */
	
	movq %r13, %rdi		/* move hexdump arr index as first arg */
	movq %r14, %rsi 	/* move position offset as second arg */
	call offset
	movq %rax, %r13		/* update the hexdump arr index w/ return value */
	addq $16, %r14		/* increment the offset by 16 */

	movq %r11, %rdi		/* set hexdump arr as first arg */
	call hex_write_string

	movq %r10, %rdi		/* set databuf arr as first arg */
	call hex_read		
	movq %rax, %r12 	/* move databuf arr length to %r12 */

	movq $0, %r13		/* reset index to 0 for new hexdump line */
	jmp .Ltop			/* continue loop */

.Ldone:
	addq $8, %rsp
	ret

/* vim:ft=gas:
 */
