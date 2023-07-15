#include <stdint.h>
#include <stdlib.h>
#include <immintrin.h>
#include <cpuid.h>
#include "util.h"
#include <time.h>

 
//constants read as expand 32-byte k, written in 4 bytes of little endian
static const uint32_t const_int_1 = 0x61707865; //apxe
static const uint32_t const_int_2 = 0x3320646e; //3 dn
static const uint32_t const_int_3 = 0x79622d32; //yb-2
static const uint32_t const_int_4 = 0x6b206574; //k et


static void sse_instrinsic_func(uint8_t* cipherPtr, const uint8_t* msgPtr, uint8_t* outputPtr, size_t i){
	
	/*	
	Instructions used from SSE2
	__m128i _mm_load_si128 (__m128i const* mem_addr)
	__m128i _mm_xor_si128 (__m128i a, __m128i b)
	void _mm_store_si128 (__m128i* mem_addr, __m128i a)
	*/

	//512 bits divided onto 4 operations
	_mm_store_si128 ((__m128i*) (cipherPtr+i*64), _mm_xor_si128 (_mm_load_si128 ((__m128i const*) (msgPtr + i*64)), _mm_load_si128 ((__m128i const*) outputPtr)));
	_mm_store_si128 ((__m128i*) (cipherPtr+i*64+16), _mm_xor_si128 (_mm_load_si128 ((__m128i const*) (msgPtr + i*64+16)), _mm_load_si128 ((__m128i const*) (outputPtr+16))));
	_mm_store_si128 ((__m128i*) (cipherPtr+i*64+32), _mm_xor_si128 (_mm_load_si128 ((__m128i const*) (msgPtr + i*64+32)), _mm_load_si128 ((__m128i const*) (outputPtr+32))));
	_mm_store_si128 ((__m128i*) (cipherPtr+i*64+48), _mm_xor_si128 (_mm_load_si128 ((__m128i const*) (msgPtr + i*64+48)), _mm_load_si128 ((__m128i const*) (outputPtr+48))));
	
}

static void avx_intrinsic_func(uint8_t* cipherPtr, const uint8_t* msgPtr, uint8_t* outputPtr, size_t i){


	#ifdef __AVX__
	
		/*
		Instructions used from AVX2
		__m256i _mm256_xor_si256 (__m256i a, __m256i b);
		__m256i _mm256_load_si256 (__m256i const * mem_addr);
		void _mm256_store_si256 (__m256i * mem_addr, __m256i a);
		*/	

		//512 bits divided onto 2 operations
		_mm256_store_si256 ((__m256i*)(cipherPtr+i*64), _mm256_xor_si256 (_mm256_load_si256 ((__m256i const*)(msgPtr+i*64)),_mm256_load_si256 ((__m256i const*)outputPtr)));
		_mm256_store_si256 ((__m256i*)(cipherPtr+i*64+32), _mm256_xor_si256 (_mm256_load_si256 ((__m256i const*)(msgPtr+i*64+32)),_mm256_load_si256 ((__m256i const*)(outputPtr+32))));

	#endif
}



static uint8_t check_avx_support()
{
    uint32_t reg_eax, reg_ebx, reg_ecx, reg_edx;

    // Call __cpuid() function with leaf set to 1 to check CPU features
    __cpuid(1, reg_eax, reg_ebx, reg_ecx, reg_edx);

    // Check if AVX bit is set (bit 28 of ECX register)
    if ((reg_ecx & (1 << 28)) != 0){
        return 1;
	}
    return 0; 
}


__attribute__((hot))
uint32_t rotate_bits_asm(uint32_t number, uint8_t i);



static void salsa20_core(uint32_t output[16], const uint32_t input[16]){


	for (int i = 0; i < 16; i++) {
		output[i] = input[i];
    }

	for (uint8_t i = 0; i < 10; i++){




		//iteration 1
		output[4] = rotate_bits_asm((output[0] + output[12]), 7) ^ output[4];
		output[9] = rotate_bits_asm((output[5] + output[1]), 7) ^ output[9]; 
		output[14] = rotate_bits_asm((output[10] + output[6]), 7) ^ output[14];
		output[3] = rotate_bits_asm((output[15] + output[11]), 7) ^ output[3];  
		      
		output[8] = rotate_bits_asm((output[0] + output[4]), 9) ^ output[8];
		output[13] = rotate_bits_asm((output[5] + output[9]), 9) ^ output[13];       
		output[2] = rotate_bits_asm((output[10] + output[14]), 9) ^ output[2];       
		output[7] = rotate_bits_asm((output[15] + output[3]), 9) ^ output[7]; 
	
		output[12] = rotate_bits_asm((output[8] + output[4]), 13) ^ output[12];
		output[1] = rotate_bits_asm((output[13] + output[9]), 13) ^ output[1];       
		output[6] = rotate_bits_asm((output[2] + output[14]), 13) ^ output[6];       
		output[11] = rotate_bits_asm((output[7] + output[3]), 13) ^ output[11];
		
		output[0] = rotate_bits_asm((output[12] + output[8]), 18) ^ output[0];
		output[5] = rotate_bits_asm((output[1] + output[13]), 18) ^ output[5];       
		output[10] = rotate_bits_asm((output[6] + output[2]), 18) ^ output[10];       
		output[15] = rotate_bits_asm((output[11] + output[7]), 18) ^ output[15];
        
        // //iteration 2 on transposed matrix
		// output[1] = rotate_bits_asm((output[0] + output[3]), 7) ^ output[1];
		// output[6] = rotate_bits_asm((output[5] + output[4]), 7) ^ output[6];       
		// output[11] = rotate_bits_asm((output[10] + output[9]), 7) ^ output[11];       
		// output[12] = rotate_bits_asm((output[15] + output[14]), 7) ^ output[12];       

		// output[2] = rotate_bits_asm((output[0] + output[1]), 9) ^ output[2];
		// output[7] = rotate_bits_asm((output[5] + output[6]), 9) ^ output[7];       
		// output[8] = rotate_bits_asm((output[10] + output[11]), 9) ^ output[8];       
		// output[13] = rotate_bits_asm((output[15] + output[12]), 9) ^ output[13]; 
	
		// output[3] = rotate_bits_asm((output[2] + output[1]), 13) ^ output[3];
		// output[4] = rotate_bits_asm((output[7] + output[6]), 13) ^ output[4];       
		// output[9] = rotate_bits_asm((output[8] + output[11]), 13) ^ output[9];       
		// output[14] = rotate_bits_asm((output[13] + output[12]), 13) ^ output[14];
		
		// output[0] = rotate_bits_asm((output[3] + output[2]), 18) ^ output[0];
		// output[5] = rotate_bits_asm((output[4] + output[7]), 18) ^ output[5];       
		// output[10] = rotate_bits_asm((output[9] + output[8]), 18) ^ output[10];       
		// output[15] = rotate_bits_asm((output[14] + output[13]), 18) ^ output[15];
		

		//REORDERING:
		output[1] = rotate_bits_asm((output[0] + output[3]), 7) ^ output[1];
		output[2] = rotate_bits_asm((output[0] + output[1]), 9) ^ output[2];
		output[3] = rotate_bits_asm((output[2] + output[1]), 13) ^ output[3];
		output[0] = rotate_bits_asm((output[3] + output[2]), 18) ^ output[0];
		output[6] = rotate_bits_asm((output[5] + output[4]), 7) ^ output[6];     
		output[7] = rotate_bits_asm((output[5] + output[6]), 9) ^ output[7];
		output[4] = rotate_bits_asm((output[7] + output[6]), 13) ^ output[4];       
    	output[5] = rotate_bits_asm((output[4] + output[7]), 18) ^ output[5];       

		output[11] = rotate_bits_asm((output[10] + output[9]), 7) ^ output[11];       
		output[12] = rotate_bits_asm((output[15] + output[14]), 7) ^ output[12];       

		
		    
		output[8] = rotate_bits_asm((output[10] + output[11]), 9) ^ output[8];       
		output[13] = rotate_bits_asm((output[15] + output[12]), 9) ^ output[13]; 
	
		output[9] = rotate_bits_asm((output[8] + output[11]), 13) ^ output[9];       
		output[14] = rotate_bits_asm((output[13] + output[12]), 13) ^ output[14];
		
		output[10] = rotate_bits_asm((output[9] + output[8]), 18) ^ output[10];       
		output[15] = rotate_bits_asm((output[14] + output[13]), 18) ^ output[15];
	}

	for (uint8_t i = 0; i < 16; i++)
	{
		output[i] += input[i]; 
	}
	
	

}



static void salsa20_crypt(size_t mlen, const uint8_t msg[mlen], uint8_t cipher[mlen], uint32_t key[8], uint64_t iv){

	//checks if avx extension is supported
	uint8_t avx_supported = check_avx_support();
 
	size_t coreCounter = mlen / 64;

	size_t restChar = mlen % 64;

	__attribute__((aligned(32)))
	uint32_t inputMatrix[16];

	//allign forcibly with a boundary of 32 bytes
	__attribute__((aligned(32))) 
	uint32_t outputMatrix[16];


	uint8_t *charPointer;
	
	//Assigning const values
	inputMatrix[0]=const_int_1;
	inputMatrix[5]=const_int_2;
	inputMatrix[10]=const_int_3;
	inputMatrix[15]=const_int_4;

	//Assigning key values
	inputMatrix[1]=key[0];
	inputMatrix[2]=key[1];
	inputMatrix[3]=key[2];
	inputMatrix[4]=key[3];
	inputMatrix[11]=key[4];
	inputMatrix[12]=key[5];
	inputMatrix[13]=key[6];
	inputMatrix[14]=key[7];


	//Assigning nonce values
	inputMatrix[6]=(iv >> 32) & 0xFFFFFFFF;
	inputMatrix[7]=iv & 0xFFFFFFFF;


    const uint8_t* msgPtr = msg;
	uint8_t* cipherPtr = cipher;
    

	//Counter value in key
	uint64_t keyCounter = 0;

	// for measuring the performance of salsa20_core_v1
	// clock_t start, end;
	// double elapsed_time = 0;
	for (size_t i = 0; i < coreCounter; i++)
	{
		//Assigning C0 and C1
		inputMatrix[8]=keyCounter & 0xFFFFFFFF;
		inputMatrix[9]=(keyCounter >> 32) & 0xFFFFFFFF;
	
		// start = clock();

		//outputMatrix contains output of core
		salsa20_core(outputMatrix,inputMatrix);

		// end = clock();
		// elapsed_time += (((double) end-start) / CLOCKS_PER_SEC) * 1000000; //result in microseconds
	
		
		uint8_t* outputPtr = (uint8_t*)outputMatrix;

		if(avx_supported){
			avx_intrinsic_func(cipherPtr, msgPtr, outputPtr, i);
		}else{
			sse_instrinsic_func(cipherPtr, msgPtr, outputPtr, i);
		}
			
		keyCounter++;
        
	}
	// double average_time = elapsed_time / coreCounter;
	// printf("Average time needed for salsa20_core: %f microseconds\n", average_time);

    
	if (restChar != 0)
	{
		inputMatrix[8]=keyCounter & 0xFFFFFFFF;
		inputMatrix[9]=(keyCounter >> 32) & 0xFFFFFFFF;

		salsa20_core(outputMatrix,inputMatrix);

		charPointer = (uint8_t*)outputMatrix;

		for (size_t j = coreCounter*64; j < mlen; j++)
		{
			cipher[j] = msg[j] ^ *charPointer;
			charPointer++;
		} 
	}
}



