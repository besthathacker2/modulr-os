; loader.s (Multiboot 2 header setup for 64-bit entry
section .multiboot
align 8
multiboot_start:
    dd 0xE85250D6                           ; Multiboot2 Magic Number
    dd 0                                    ; Architecture 0 (Protected i386 mode)
    dd multiboot_end - multiboot_start      ; Header length
    dd -(0xE85250D6 + 0 + (multiboot_end - multiboot_start)) ; Checksum
    dw 0
    dw 0
    dd 8
multiboot_end:

section .bss
align 16
stack_bottom:
    resb 65536                              ; 64 KB initial hardware bootstrap stack
stack_top:

section .text
bits 32                                     ; The CPU starts executing instructions in 32-bit mode
global _start
extern init_64bit_paging
extern kernelMain

_start:
    mov esp, stack_top                      ; Point to our safety bootstrap stack

    ; 1. Call our C++ memory table builder loop
    call init_64bit_paging

    ; 2. Load the top-level PML4 physical memory address directly into CR3 register
    extern pml4
    mov eax, pml4
    mov cr3, eax

    ; 3. Enable PAE (Physical Address Extension) in CR4 - Required for 64-bit paging
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; 4. Set the Long Mode Active bit inside the EIMS MSR register block
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; 5. Toggle the primary Paging bit inside the CR0 control register
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; 6. Perform a far jump to force execution into 64-bit code space segment
    jmp 0x08:long_mode_start

bits 64                                     ; The CPU is now running natively in 64-bit mode!
long_mode_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Update our Stack Pointer to target the top of our newly mapped virtual memory spectrum
    mov rsp, stack_top
    
    ; Jump to the 64-bit kernel logic
    call kernelMain

.hang:
    cli
    hlt
    jmp .hang
