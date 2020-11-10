.386
.model flat, stdcall
option casemap : none
include     c:\masm32\include\windows.inc
include     c:\masm32\include\kernel32.inc
include     c:\masm32\include\masm32.inc
includelib  c:\masm32\lib\kernel32.lib
includelib  c:\masm32\lib\masm32.lib

NumbToStr	PROTO :DWORD,:DWORD
main	PROTO

.data

buff 	 db 11 dup(?)

.code

start:

	invoke main

	invoke  NumbToStr, ebx, ADDR buff
	invoke  StdOut, eax
	invoke  ExitProcess, 0

main PROC
	mov ebx, 99
	ret

main ENDP

NumbToStr PROC uses ebx x:DWORD,buffer:DWORD

	mov	ecx, buffer
	mov	eax, x
	mov	ebx, 10
	add	ecx, ebx
@@:
	xor	edx, edx
	div	ebx
	add	edx, 48
	mov	BYTE PTR[ecx], dl
	dec	ecx
	test	eax, eax
	jnz	@b
	inc	ecx
	mov	eax, ecx
	ret
NumbToStr	ENDP

END	start
