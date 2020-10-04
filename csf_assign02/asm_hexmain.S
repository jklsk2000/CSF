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

	.globl offset
offset:
	pushq %r12				/* save callee saved registers */
	pushq %r13
	pushq %r14
	pushq %rbp
	subq $16, %rsp
	movq %rsp, %rbp

	movq %rdi, %rdx			/* move hexdump arr index to %rdx */
	movq %rsi, %rdi 		/* move offset value to %rdi */
	movq %rbp, %rsi 		/* pass the array to %rsi */
	call hex_format_offset	/* call hex_format_offset function */

	movq $0, %rcx 			/* use %rcx as loop counter */
	movq $hexdump, %r11		/* move addr of hexdump arr to %r11 */

.LoffsetLoop:
	cmpq $8, %rcx			/* loop of count 8 */
	jge .LoffsetDone		/* if counter is >= 8, stop */

	movq (%rbp, %rcx), %r8	/* store one offset elt temporarily */
	movq %r8, (%r11, %rdx)	/* transfer offset elt to hexdump arr */

	incq %rdx				/* increment hexdump arr index */
	incq %rcx				/* increment loop counter */
	jmp .LoffsetLoop		/* continue loop */

.LoffsetDone:
	movq $sColon, %r8		
	movq %r8, (%r11, %rdx)	/* add a ":" at the end of offset */
	incq %rdx				/* increment hexdump arr index */

	movq %rdx, %rax			/* move hexdump arr index to %rax to return */
	addq $16, %rsp			/* realign stack */
	popq %rbp			
	popq %r14
	popq %r13
	popq %r12				/* restore callee saved registers */
	ret

/*------------------------------------------------------------------------------------*/

/*
 * Register Use:
 *
 *
 */
	.globl hexString
hexString:
	pushq %r12				/* save callee saved registers */
	pushq %r13
	pushq %r14
	subq $8, %rsp			/* stack alignment */

	movq $0, %rcx 			/* set %rcx as loop counter */
	movq %rdi, %rdx 		/* store hexdump arr index in %rdx */
	movq %rsi, %r13 		/* store databuf arr len in %r13 */
	movq $databuf, %r10 	/* store addr of databuf in %r10 */
	movq $hexdump, %r11 	/* store addr of hexdump in %r11 */
	movq $hexdigit, %r12 	/* store addr of hexdigit in %r12 */
	
	movq $0, %r8 			/* %r8 will decide if this is the last hexdump line */
	cmpq $16, %r13			/* compare databuf arr len to 16 */
	je .LhexLoop			/* if databuf arr len is not 16 -> last line */
	movq $1, %r8			/* move 1 to %r8 to indicate that this is last */

.LhexLoop:
	cmpq $16, %rcx 			/* compare %rcx to 16: loop of length 16 */
	jge .LhexDone			/* if %rcx >= 16 -> done with loop */

	movq $sSpace, %r9 		/* move space char to temp */
	movq %r9, (%r11, %rdx)	/* insert space in hexdump arr */
	incq %rdx

	cmpq $0, %r8
	je .LhexCall			/* if not last line, jmp to .LhexCall */
	cmpq %rcx, %r13
	jle .LhexCall			/* if last line and nothing more to print */

.LhexCall:
	movq (%r10, %rcx), %rdi	/* pass one elt of databuf as first arg */
	movq %r12, %rsi			/* pass addr of hexdigit as second arg */
	call hex_format_byte_as_hex

	movq 0(%r12), %r9		/* move 1st hex digit to temp */
	movq %r9, (%r11, %rdx)	/* then move it to hexdump arr */
	incq %rdx
	movq 1(%r12), %r9		/* move 2nd hex digit to temp */
	movq %r9, (%r11, %rdx)	/* then move it to hexdump arr */
	incq %rdx
	jmp .LhexNext			/* jump to continue loop */

.LhexFill:					/* last line + nothing more to print */
	movq $sSpace, %r9 		/* move space char to temp */
	movq %r9, (%r11, %rdx)	/* insert space in hexdump arr */
	incq %rdx
	movq %r9, (%r11, %rdx)	/* insert space in hexdump arr */
	incq %rdx

.LhexNext:
	incq %rcx				/* increase loop count */
	jmp .LhexLoop			/* continue through loop */

.LhexDone:
	movq %rdx, %rax 		/* move hexdump arr index to %rax to return */
	addq $8, %rsp			/* realign stack */
	popq %r14
	popq %r13
	popq %r12				/* restore callee saved registers */
	ret

/*------------------------------------------------------------------------------------*/

	.globl printable
printable:
	pushq %r12				/* save callee saved registers */
	pushq %r13
	pushq %r14
	subq $8, %rsp			/* stack alginment */

	movq %rdi, %rdx			/* move hexdump arr index to %rdx */
	movq %rsi, %r8			/* move databuf arr len to %r8 */
	movq $databuf, %r10 	/* move databuf arr addr to %r10 */
	movq $hexdump, %r11		/* move hexdump arr addr to %r11 */

	movq $sSpace, %r9 		/* move space char to temp */
	movq %r9, (%r11, %rdx)	/* move space in to hexdump arr */
	incq %rdx
	movq %r9, (%r11, %rdx)	/* move space in to hexdump arr */
	incq %rdx

	movq $0, %rcx 			/* set %rcx to loop counter */

.LprintLoop:
	cmpq $16, %rcx			/* loop counter should only go up to 16 */
	jge .LprintDone			/* if loop count >= 16, then done */
	cmpq %rcx, %r13			/* if databuf arr len is <= loop counter */
	jle .LprintDone			/* break from the loop */

	movq (%r10, %rcx), %rdi /* set elt of databuf arr as first arg */
	call hex_to_printable
	movq %rax, (%r11, %rdx)	/* put result of fxn call to hexdump arr */
	incq %rdx

	incq %rcx				/* increment loop counter */
	jmp .LprintLoop			/* continue loop */

.LprintDone:
	movq %rdx, %rax			/* set hexdump arr index to %rax to be returned */
	addq $8, %rsp			/* realgin stack */
	popq %r14
	popq %r13
	popq %r12				/* restore callee saved registers */
	ret

/*------------------------------------------------------------------------------------*/

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

	movq %r13, %rdi 	/* move hexdump arr index as first arg */
	movq %r12, %rsi		/* move length of databuf as second arg */
	call hexString
	movq %rax, %r13		/* update the hexdump arr index w/ return value */

	movq %r13, %rdi		/* move hexdump arr index as first arg */
	movq %r12, %rsi 	/* move databuf arr len as second arg */
	call printable		
	movq %rax, %r13		/* update the hexdump arr index w/ return value */
	
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
