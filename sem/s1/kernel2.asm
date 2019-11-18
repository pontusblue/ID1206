bits 32
global start

section .text

start:
	mov word [0xb8244], 0x8b50		; print P
	mov word [0xb8246], 0x8b6f		; print o
	mov word [0xb8248], 0x8b6e		; print n
	mov word [0xb824a], 0x8b74		; print t
	mov word [0xb824c], 0x8b75		; print u
	mov word [0xb824e], 0x8f73		; print s
	hlt
