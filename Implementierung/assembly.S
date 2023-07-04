
;static void transponse_1( uint32_t array[16]){
transpose_asm:
xor rax,rax

;swapping array[2] with array[5]
mov ax, [rdi+4]
xchg ax, [rdi+16]
mov [rdi+4], ax

;swapping array[3] with array[9]
mov ax, [rdi+8]
xchg ax, [rdi+32]
mov [rdi+8], ax

;swapping array[4] with array[13]
mov ax, [rdi+12]
xchg ax, [rdi+48]
mov [rdi+12], ax

;swapping array[7] with array[10]
mov ax, [rdi+24]
xchg ax, [rdi+36]
mov [rdi+24], ax

;swapping array[8] with array[14]
mov ax, [rdi+28]
xchg ax, [rdi+52]
mov [rdi+28], ax

;swapping array[12] with array[15]
mov ax, [rdi+44]
xchg ax, [rdi+56]
mov [rdi+44], ax

ret


;static uint32_t rotate_bits_1(uint32_t number, uint8_t i)
rotate_bits_asm:
xor rax, rax
xor rcx, rcx
mov ax, [rdi]
mov cl, [rsi]
rol ax, cl
ret