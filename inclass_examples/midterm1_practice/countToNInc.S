.section .rodata

sPromptVal: .string "Enter maximum: "
sPromptInc: .string "Enter increment: "
sInputFmt: .string "%lu\n"

.section .data

valIn: .space 4
incIn: .space 4

.section .text
    .globl main

main:
    subq $8, %rsp

    movq $sPromptVal, %rdi
    call printf

    movq $sInputFmt, %rdi
    movq $valIn, %rsi
    call scanf

    movq $sPromptInc, %rdi
    call printf

    movq $sInputFmt, %rdi
    movq $incIn, %rsi
    call scanf

    movq $1, %r12

.Ltop:

    movq $sInputFmt, %rdi
    movq %r12, %rsi
    call printf
    addq incIn, %r12
    cmpq valIn, %r12
    jle .Ltop

    addq $8, %rsp
    ret