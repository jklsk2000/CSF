.section .rodata

sPrompt: .string "Enter an integer: "
sInputFmt: .string "%lu\n"

.section .data

val: .space 4

.section .text
    .globl main

main:
    subq $8, %rsp

    movq $sPrompt, %rdi
    call printf

    movq $sInputFmt, %rdi
    movq $val, %rsi
    call scanf

    movq $1, %r12

.Ltop:

    movq $sInputFmt, %rdi
    movq %r12, %rsi
    call printf
    incq %r12
    cmpq val, %r12
    jle .Ltop

    addq $8, %rsp
    ret