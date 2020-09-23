/* hello.S */

.section .rodata

sHelloMsg: .string "Hello, world\n"
sAnotherMsg: .string "This is my first assembly language program!\n"

.section .text

	.globl main
main:
	subq $8, %rsp
	movq $sHelloMsg, %rdi
	call printf
	movq $sAnotherMsg, %rdi
	call printf

	addq $8, %rsp
	ret