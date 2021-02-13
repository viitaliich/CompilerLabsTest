.386
.model flat, stdcall
option casemap : none
include     c:\masm32\include\windows.inc
include     c:\masm32\include\kernel32.inc
include     c:\masm32\include\masm32.inc
includelib  c:\masm32\lib\kernel32.lib
includelib  c:\masm32\lib\masm32.lib

NumbToStr	PROTO :DWORD,:DWORD
sum	PROTO
main	PROTO

.data

buff 	 db 11 dup(?)

.code

start:

	invoke main
	invoke  ExitProcess, 0

sum PROC
	push ebp
	mov ebp, esp
	mov ebx, 5
	mov [ebp + 8], ebx
	mov ebx, 0

sum ENDP

main PROC
	push ebp
	mov ebp, esp
	mov ebx, 1
	push ebx
	call sum
	add esp, 4
	mov ebx, 0

main ENDP

NumbToStr PROC uses ebx x:DWORD,buffer:DWORD

	mov	ecx, buffer
	mov	eax, x
	mov	ebx, 10
	add	ecx, ebx

	cmp eax, 0
	jge lbl
	neg eax

@@:
	xor	edx, edx
	div	ebx
	add	edx, 48
	mov	BYTE PTR[ecx+1], dl
	dec	ecx
	test	eax, eax
	jnz	@b
	mov	BYTE PTR[ecx+1], '-'
	inc	ecx
	mov	eax, ecx
	jmp stp

lbl:
	xor	edx, edx
	div	ebx
	add	edx, 48
	mov	BYTE PTR[ecx], dl
	dec	ecx
	test	eax, eax
	jnz	lbl
	inc	ecx
	mov	eax, ecx
stp:
	ret

NumbToStr	ENDP

END	start
