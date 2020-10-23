; Ksawery Chodyniecki
; setPixel

section .text
global paintASM

paintASM:
    push    rbp
    mov     rbp, rsp
    mov     r9, rdx             ; address of text to r9
    mov     rdx, 0              ; rdx - module_iterator
    mov     r10, rcx            ; r10 - x (0 + offset)

paint_loop:
    mov     al, BYTE[r9]        ; gets a sign
    cmp     al, 0
    jz      end                 ; jump to end if \0
    cmp     al, '1'
    jne     do_if_zero          ; jump if not '1'
    mov     rdx, 0              ; resets module_iterator
module_loop:
; set_mask
    mov     rcx, 0x0007
    and     rcx, r10
    mov     r8, 0x0080
    shr     r8, cl
    not     r8
; get_pixel
    mov     rcx, r10
    shr     rcx, 3
; set_pixel
    mov     rax, rdi
    add     rax, rcx
    and     QWORD[rax], r8
; module_loop_end
    inc     r10                 ; increment x
    inc     rdx
    mov     rax, rsi
    cmp     rdx, rax
    jb      module_loop
paint_loop_end:
    inc     r9
    jmp     paint_loop

do_if_zero:
    mov     rax, rsi
    add     r10, rax            ; add module_width to x
    inc     r9
    jmp     paint_loop

end:
    pop     rbp
    ret
