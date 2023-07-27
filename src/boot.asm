	[bits 16]
	[org 0x7c00]

    start:
		jmp short kod
		nop

		db "sillyOS "	; nazwa OS i wersja OEM (8B)
		dw 512		; bajtów/sektor (2B)
		db 1		; sektory/jednostkę alokacji (1B)
		dw 1		; zarezerwowane sektory (2B)
		db 2		; liczba tablic alokacji (1B)
		dw 224		; liczba pozycji w katalogu głównym (2B)
				; 224 to typowa wartość
		dw 2880		; liczba sektorów (2B)
		db 0f0h		; Media Descriptor Byte (1B)
		dw 9		; sektory/FAT (2B)
		dw 18		; sektory/ścieżkę (2B)
		dw 2		; liczba głowic (2B)
		dd 0		; liczba ukrytych sektorów (4B)
		dd 0		; liczba sektorów (część 2),
				; jeśli wcześniej było 0 (4B)
		db 0		; numer dysku (1B)
		db 0		; zarezerwowane (1B)
		db 0		; rozszerzona sygnatura bloku ładującego
		dd 0bbbbddddh	; numer seryjny dysku (4B)
		db "           "; etykieta (11B)
		db "FAT 12  "	; typ FAT (8B), zwykle  "FAT 12  "

        %define KERNEL_POS 0x9000


	kod:

		; Set up the segment registers
		xor ax, ax
		mov ds, ax
		mov es, ax
		mov ss, ax

		; Prepare stack
		mov sp, 0x7c00
		mov bp, sp

		; Get boot disk
		mov [BOOT_DISK], dl

		xor ax, ax                          
		mov es, ax
		mov ds, ax
		mov bp, 0x8000
		mov sp, bp

		mov bx, KERNEL_POS
		mov dh, 0x20

		mov ah, 0x02
		mov al, dh 
		mov ch, 0x00
		mov dh, 0x00
		mov cl, 0x02
		mov dl, [BOOT_DISK]
		int 0x13                ; no error management, do your homework!


		cli
		xor ax, ax
		mov ds, ax
		cli
		gdt: lgdt [gdt+1]
		db 0,0xfa
		jmp b
		dw 0xFFFF            ; Limit
		.zero: dw 0x0000            ; Base (low 16 bits)
		db 0x00                ; Base (mid 8 bits)
		.a: db 10011010b        ; Access
		db 11001111b        ; Granularity
		db 0x00                ; Base (high 8 bits)
		b: inc ax
		lmsw ax
		jmp 0x08:wow
		[bits 32]
		wow: and byte [gdt.a], ~(1<<3)
		mov al, 0x08
		mov ds, ax
		mov es, ax
		mov ss, ax

		jmp pm32
    	
	BOOT_DISK: db 0

	pm32:
		; Load the kernel loader and the kernel all together!
		mov al, [BOOT_DISK]
		jmp KERNEL_POS

		.hang:
			cli
			hlt
			jmp .hang


	[bits 16]
	times 510-($-$$) db 0
	dw 0xaa55
