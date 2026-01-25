; hello.asm (x86-64 Linux, NASM)
global _start

section .data
    msg db "Hello, world!", 10
    len equ $ - msg

section .text
_start:
    ; write(1, msg, len)
    mov     rax, 1          ; syscall: sys_write
    mov     rdi, 1          ; fd = stdout
    mov     rsi, msg        ; buf
    mov     rdx, len        ; count
    syscall

    ; exit(0)
    mov     rax, 60         ; syscall: sys_exit
    xor     rdi, rdi        ; status = 0
    syscall
 
