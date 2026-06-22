; loader.s - 64-bit Multiboot2 with Framebuffer Request
section .multiboot
align 8
multiboot_start:
    dd 0xE85250D6                           ; Multiboot2 Magic Number
    dd 0                                    ; Architecture 0 (Protected i386 mode)
    dd multiboot_end - multiboot_start      ; Header length
    dd -(0xE85250D6 + 0 + (multiboot_end - multiboot_start)) ; Checksum

    ; Tag: Request Framebuffer
    align 8
    dw 5                                    ; Tag type 5 (Framebuffer)
    dw 0                                    ; Flags
    dd 20                                   ; Tag size
    dd 1024                                 ; Requested Width
    dd 768                                  ; Requested Height
    dd 32                                   ; Requested Bits Per Pixel (BPP)

    ; End Tag
    align 8
    dw 0
    dw 0
    dd 8
multiboot_end:

section .bss
align 16
stack_bottom:
    resb 65536                              ; 64 KB initial stack
stack_top:

section .text
bits 32
global _start
extern init_64bit_paging
extern kernelMain

_start:
    mov esp, stack_top
    
    ; Push the Multiboot2 magic number and information structure pointer to pass to C++
    push ebx                                ; EBX points to the Multiboot information structure
    push eax                                ; EAX contains the Magic Number

    call init_64bit_paging

    ; Load Page Tables into CR3
    extern pml4
    mov eax, pml4
    mov cr3, eax

    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Set Long Mode Active Bit in MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable Paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Jump to 64-bit Long Mode
    jmp 0x08:long_mode_start

bits 64
long_mode_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, stack_top

    ; Pop our Multiboot parameters into 64-bit registers to pass to kernelMain(magic, addr)
    pop rdi                                 ; First argument (Magic Number)
    pop rsi                                 ; Second argument (Structure Pointer)
    
    call kernelMain

.hang:
    cli
    hlt
    jmp .hang
