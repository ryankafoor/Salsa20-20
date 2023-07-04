.intel_syntax noprefix
.global rotate_bits_asm


.text
//static uint32_t rotate_bits_1(uint32_t number, uint8_t i)
rotate_bits_asm:
mov eax, edi
mov cl,  sil
rol eax, cl
ret 