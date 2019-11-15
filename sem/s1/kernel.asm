bits 32
global start

section .text

start:
	mov word [0xb8000], 0x8b50		; print P
	mov word [0xb8002], 0x8b6f		; print o
	mov word [0xb8004], 0x8b6e		; print n
	mov word [0xb8006], 0x8b74		; print t
	mov word [0xb8008], 0x8b75		; print u
	mov word [0xb800a], 0x8f73		; print s
	hlt
