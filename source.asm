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
	invoke  ExitProcess, 0

main PROC
	push ebp
	mov ebp, esp
	mov ebx, 1
	push ebx
_label0:
	mov ebx, 1
	mov [ebp + -4], ebx
	push ebx
	mov ebx, 5
	pop ecx
	cmp ecx, ebx
	mov ebx, 0
	setl bl
	cmp ebx, 0
	je _label1
	mov ebx, 5
	push ebx
	mov ebx, 0
	add esp, 4
	mov ebx, 1
	mov [ebp + -4], ebx
	push ebx
	mov ebx, 1
	pop ecx
	add ebx, ecx
	jmp _label0
_label1:
	mov ebx, 0
	invoke  NumbToStr, ebx, ADDR buff
	invoke  StdOut, eax
	mov esp, ebp
	pop ebp
	ret

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
