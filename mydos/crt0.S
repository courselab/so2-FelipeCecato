.code16gcc
.global _start
	
.section .text

jmp _start


_start:
	call main
	ret
