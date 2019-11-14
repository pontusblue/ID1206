bits 16

start:
	mov ax, 0x07c0		; 0x07c00 is where we are
	add ax, 0x20		; add 0x20 (when shifter 512)
	mov ss, ax		; set the stack segment
	mov sp, 0x1000		; set the stack pointer

	mov ax, 0x07c0		; set data segment...
	mov ds, ax		; more about this later

	mov si, msg		; pointer to the message in SI
	mov ah, 0x0e		; print char BIOS procedure

.next:
	lodsb			; next byte to AL, increment SI
	cmp al, 0		; if the byte is zero
	je .done		; jump do done
	int 0x10		; invoke the BIOS system call using interrupt
	jmp .next		; loop

.done:
	jmp $			; loop forever

msg:	db 'Hello', 0		; the string we want to print

	times 510-($-$$) db 0	; fill up to 510 bytes
	dw 0xaa55		; master boot record signature
