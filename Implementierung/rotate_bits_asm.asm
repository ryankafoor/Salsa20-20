.intel_syntax noprefix
.global rotate_bits_asm

//static uint32_t rotate_bits_1(uint32_t number, uint8_t i)
rotate_bits_asm:
xor rax, rax
xor rcx, rcx
mov eax, [rdi]
mov cl, [rsi]
rol eax, cl
ret 