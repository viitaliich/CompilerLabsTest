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
	push ebp
	mov ebp, esp
	mov ebx, 8
	push ebx
	mov ebx, 0
	mov ebx, 0
	cmp ebx, 0
	je _label0
	mov ebx, [ebp + -4]
	mov eax, ebx
	cdq
	mov ebx, 10
	mov ecx, ebx
	idiv ecx
	mov ebx, edx
	cmp eax, 0
	jge _label1
	neg ebx
_label0:
	mov [ebp + -4], ebx
	mov ebx, 0
	jmp _label2
_label1:
_label2:
	mov ebx, [ebp + -4]
	push ebx
	mov ebx, 1
	pop ecx
	add ebx, ecx
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
