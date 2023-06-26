


static const uint32_t const1 = 0x61707865;
static const uint32_t const2 = 0x3320646e;
static const uint32_t const3 = 0x79622d32;
static const uint32_t const4 = 0x6b206574;

//TODO: C implementation with intrinsics


//assembly can be used here to optimize this function (rol)
__attribute__((hot))
//__attribute__((const))
__attribute__((always_inline))
static uint32_t rotate_bits_2(uint32_t number, uint8_t i){
  return (number << i) | (number >> (32 -i));
}


//xchg
__attribute__((hot))
__attribute__((always_inline)) 
static void transponse_2( uint32_t array[16]){
  

  uint32_t temp;

  temp = array[1];
  array[1] = array[4];
  array[4] = temp;

  temp = array[2];
  array[2] = array[8];
  array[8] = temp;

  temp = array[3];
  array[3] = array[12];
  array[12] = temp;

  temp = array[6];
  array[6] = array[9];
  array[9] = temp;

  temp = array[7];
  array[7] = array[13];
  array[13] = temp;

  temp = array[11];
  array[11] = array[14];
  array[14] = temp;
    
}

static void salsa20_core_2(uint32_t output[16], const uint32_t input[16]){





/*
  for (int i = 0; i < 16; i++) {
    output[i] = input[i];
}

  for (uint8_t i = 0; i < 20; i++)
  {
    output[4] = rotate_bits_2((output[0] + output[12]), 7) ^ output[4];
    output[9] = rotate_bits_2((output[5] + output[1]), 7) ^ output[9];       
    output[14] = rotate_bits_2((output[10] + output[6]), 7) ^ output[14];       
    output[3] = rotate_bits_2((output[15] + output[11]), 7) ^ output[3];       

    output[8] = rotate_bits_2((output[0] + output[4]), 9) ^ output[8];
    output[13] = rotate_bits_2((output[5] + output[9]), 9) ^ output[13];       
    output[2] = rotate_bits_2((output[10] + output[14]), 9) ^ output[2];       
    output[7] = rotate_bits_2((output[15] + output[3]), 9) ^ output[7]; 
  
    output[12] = rotate_bits_2((output[8] + output[4]), 13) ^ output[12];
    output[1] = rotate_bits_2((output[13] + output[9]), 13) ^ output[1];       
    output[6] = rotate_bits_2((output[2] + output[14]), 13) ^ output[6];       
    output[11] = rotate_bits_2((output[7] + output[3]), 13) ^ output[11];
    
    output[0] = rotate_bits_2((output[12] + output[8]), 18) ^ output[0];
    output[5] = rotate_bits_2((output[1] + output[13]), 18) ^ output[5];       
    output[10] = rotate_bits_2((output[6] + output[2]), 18) ^ output[10];       
    output[15] = rotate_bits_2((output[11] + output[7]), 18) ^ output[15];
  
    transponse_2(output);
  }2

  for (uint8_t i = 0; i < 16; i++)
  {
    output[i] += input[i]; 
  }
  
  */

}

static void salsa20_crypt_2(size_t mlen, const uint8_t msg[mlen], uint8_t cipher[mlen], uint32_t key[8], uint64_t iv){

  
  size_t coreCounter = mlen / 64;

  size_t restChar = mlen % 64;

  __attribute__((aligned(16)))
  uint32_t inputMatrix1[16];

  __attribute__((aligned(16)))
  uint32_t outputMatrix1[16];

  __attribute__((aligned(16)))
  uint32_t inputMatrix2[16];

  __attribute__((aligned(16)))
  uint32_t outputMatrix2[16];




  uint8_t *charPointer;
  
  //Assigning const values
  inputMatrix1[0]=const1;
  inputMatrix1[5]=const2;
  inputMatrix1[10]=const3;
  inputMatrix1[15]=const4;

  //Assigning key values
  inputMatrix1[1]=key[0];
  inputMatrix1[2]=key[1];
  inputMatrix1[3]=key[2];
  inputMatrix1[4]=key[3];
  inputMatrix1[11]=key[4];
  inputMatrix1[12]=key[5];
  inputMatrix1[13]=key[6];
  inputMatrix1[14]=key[7];


  //Assigning nonce values
  inputMatrix1[6]=iv & 0xFFFFFFFF;
  inputMatrix1[7]=(iv >> 32) & 0xFFFFFFFF;

  for(int i=0;i<16;i++){
    inputMatrix2[i] = inputMatrix1[i];
  }


  //Counter value in key
  uint64_t keyCounter = 0;

  for (size_t i = 0; i < coreCounter; i++)
  {
    //Assigning C0 and C1
    inputMatrix1[8]=keyCounter & 0xFFFFFFFF;
    inputMatrix1[9]=(keyCounter >> 32) & 0xFFFFFFFF;
    keyCounter++;
    inputMatrix2[8]=keyCounter & 0xFFFFFFFF;
    inputMatrix2[9]=(keyCounter >> 32) & 0xFFFFFFFF;
    

    //outputMatrix contains output of core
    salsa20_core_2(outputMatrix1,inputMatrix1);
    salsa20_core_2(outputMatrix2,inputMatrix2);  

    
    charPointer = (uint8_t*)outputMatrix;

    //initializing key for each byte and pointer to outputMatrix
      
      for (size_t j = 0; j < 64; j++)
      {
        cipher[i*64+j] = msg[i*64+j] ^ *charPointer;
        charPointer++;
      }
     
      //for (size_t simdCounter = 0; simdCounter < 4; simdCounter++)
      //{
      //__m128i simdReg1 = _mm_loadu_si128((__m128i_u *)msg);
      //__m128i simdReg2 = _mm_loadu_si128((__m128i_u *)outputMatrix);

      //}
      
      
      
    keyCounter++;
  }

  if (restChar != 0)
  {
    inputMatrix[8]=keyCounter & 0xFFFFFFFF;
    inputMatrix[9]=(keyCounter >> 32) & 0xFFFFFFFF;

    salsa20_core_1(outputMatrix,inputMatrix);

    charPointer = (uint8_t*)outputMatrix;

    //Check values of OutputMatrix
    //printf("Output Matrix :\n");
    //for ( int x = 0 ; x < 16 ; x++){
      //printf("%u\n",outputMatrix[x]);
    //}
    for (size_t j = coreCounter*64; j < mlen; j++)
    {
      cipher[j] = msg[j] ^ *charPointer;
      charPointer++;
    } 
  }
  
}


