#define N 10

.section .rodata

sSum: .string "The sum is %lu\n"

.section .text
    .globl main

main:
    subq $8, %rsp

    movl $0, %ecx
    movl $0, %eax

.Ltop:

    addl %ecx, %eax
    incl %ecx
    cmpl $N, %ecx
    jl .Ltop

    movl $sSum, %edi
    movl %eax, %esi
    call printf

    addq $8, %rsp
    ret