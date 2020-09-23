/* months.S */
.section .rodata
sPromptMsg:  .string "Enter month number (1-12): "
sInputFmt:   .string "%d"
s31DaysMsg:  .string "31 days\n"
s30DaysMsg:  .string "30 days\n"
sFebMsg:     .string "28 or 29 days\n"
sDefaultMsg: .string "not a valid month\n"

.section .bss
monthNum: .space 4

.section .text

	.globl main
main:
	subq $8, %rsp

	/* prompt user to enter a month number */
	movq $sPromptMsg, %rdi
	call printf

	/* read input from user */
	movq $sInputFmt, %rdi
	movq $monthNum, %rsi
	call scanf

	/* check whether month number is in range, if not, jump to
	 * the default case */
	movl monthNum, %esi
	cmpl $1, %esi
	jl .LDefaultCase
	cmpl $12, %esi
	jg .LDefaultCase

	/* convert to 0-indexed and jump to appropriate case */
	dec %esi
	jmp *.LJumpTable(,%esi,8)

.L31DaysCase:
	movq $s31DaysMsg, %rdi
	call printf
	jmp .LSwitchDone

.L30DaysCase:
	movq $s30DaysMsg, %rdi
	call printf
	jmp .LSwitchDone

.LFebCase:
	movq $sFebMsg, %rdi
	call printf
	jmp .LSwitchDone

.LDefaultCase:
	movq $sDefaultMsg, %rdi
	call printf

.LSwitchDone:
	addq $8, %rsp
	ret

.LJumpTable:
	.quad .L31DaysCase
	.quad .LFebCase
	.quad .L31DaysCase
	.quad .L30DaysCase
	.quad .L31DaysCase
	.quad .L30DaysCase
	.quad .L31DaysCase
	.quad .L31DaysCase
	.quad .L30DaysCase
	.quad .L31DaysCase
	.quad .L30DaysCase
	.quad .L31DaysCase

/*
vim:ft=gas:
*/
