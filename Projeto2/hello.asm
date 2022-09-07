global _start

    section .text
_start:
    mov eax, 4                  ; system call for write
    mov ebx, 1  
    mov ecx, message 
    mov edx, 13
    int 80h
    mov eax, 1
    mov ebx, 0
    int 80h

    section .data
message: 
    db "hello",10      ; note the newline at the end

    ; nasm -felf hello.asm -o hello.o
    ; ld -m elf_i386 hello.o -o hello
    ; ./hello