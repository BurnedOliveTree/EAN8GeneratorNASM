; Ksawery Chodyniecki
; kodowanie ciągu znaków

section .data
    cypher:    db  '0001101001100100100110111101010001101100010101111011101101101110001011', 70

section .text
global encodeASM

encodeASM:
    push    rbp
    mov     rbp, rsp
    add     rsi, 3              ; moves 3 sings beyond the start of binar
    mov     r9, 0

encode_loop:
    mov     rax, 0
    mov     al, BYTE[rdi]       ; get a sign

    cmp     al, 0
    je      end                 ; jump to end if \0

    cmp     r9, 4               ; checks if its the middle ground arleady
    jne     encode_loop_cd
    add     rsi, 5
    mov     r11b, 1

encode_loop_cd:
    inc     rdi
    sub     al, '0'             ; changes char into int
    mov     r8b, al
    shl     al, 3
    sub     al, r8b
    mov     rcx, cypher
    add     rcx, rax            ; gets the start of the correct number from cypher
    mov     r8b, 0
seven_loop:
    mov     al, BYTE[rcx]
    cmp     r11b, 1             ; checks the r11b flag if it should be C arleady
    jne     encode_A
    cmp     al, '0'
    je      change_to_one
change_to_zero:
    sub     al, 2               ; -2 because -2+1=-1 to avoid additionial jump
change_to_one:
    add     al, 1
encode_A:
    mov     BYTE[rsi], al
    inc     rcx
    inc     rsi
    inc     r8b
    cmp     r8b, 7
    jb      seven_loop
    inc     r9
    jmp     encode_loop

end:
    pop     rbp
    ret
