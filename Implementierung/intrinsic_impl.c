


//TODO: C implementation with intrinsics

//Maybe much efficienter bit-rotating function?

//__attribute__((hot))
//__attribute__((always_inline))
static uint32_t rotate_bits_2(uint32_t number, uint8_t i){
  return (number << i) | (number >> (32 -i));
}

//__attribute__((hot))
//__attribute__((always_inline))
static void transponse_2( uint32_t array[16]){
  
  uint32_t temp[16];

  temp[0] = array[0];
  temp[4] = array[1];
  temp[8] = array[2];
  temp[12] = array[3];
  temp[1] = array[4];
  temp[5] = array[5];
  temp[9] = array[6];
  temp[13] = array[7];
  temp[2] = array[8];
  temp[6] = array[9];
  temp[10] = array[10];
  temp[14] = array[11];
  temp[3] = array[12];
  temp[7] = array[13];
  temp[11] = array[14];
  temp[15] = array[15];

  for(int i = 0; i < 16; i++) {
    array[i] = temp[i];
  }
}

static void salsa20_core_2(uint32_t output[16], const uint32_t input[16]){

  for (int i = 0; i < 16; i++) {
    output[i] = input[i];
}

  for (uint8_t i = 0; i < 20; i++)
  {
    output[4] = rotate_bits_1((output[0] + output[12]), 7) ^ output[4];
    output[9] = rotate_bits_1((output[5] + output[1]), 7) ^ output[9];       
    output[14] = rotate_bits_1((output[10] + output[6]), 7) ^ output[14];       
    output[3] = rotate_bits_1((output[15] + output[11]), 7) ^ output[3];       

    output[8] = rotate_bits_1((output[0] + output[4]), 9) ^ output[8];
    output[13] = rotate_bits_1((output[5] + output[9]), 9) ^ output[13];       
    output[2] = rotate_bits_1((output[10] + output[14]), 9) ^ output[2];       
    output[7] = rotate_bits_1((output[15] + output[3]), 9) ^ output[7]; 
  
    output[12] = rotate_bits_1((output[8] + output[4]), 13) ^ output[12];
    output[1] = rotate_bits_1((output[13] + output[9]), 13) ^ output[1];       
    output[6] = rotate_bits_1((output[2] + output[14]), 13) ^ output[6];       
    output[11] = rotate_bits_1((output[7] + output[3]), 13) ^ output[11];
    
    output[0] = rotate_bits_1((output[12] + output[8]), 18) ^ output[0];
    output[5] = rotate_bits_1((output[1] + output[13]), 18) ^ output[5];       
    output[10] = rotate_bits_1((output[6] + output[2]), 18) ^ output[10];       
    output[15] = rotate_bits_1((output[11] + output[7]), 18) ^ output[15];
  
    transponse_1(output);
  }

  for (uint8_t i = 0; i < 16; i++)
  {
    output[i] += input[i]; 
  }
  
  

}

static void salsa20_crypt_2(size_t mlen, const uint8_t msg[mlen], uint8_t cipher[mlen], uint32_t key[8], uint64_t iv){
  
  size_t coreCounter = mlen / 64;

  size_t restChar = mlen % 64;

  
  uint32_t inputMatrix[16];
  uint32_t outputMatrix[16];

  uint8_t *charPointer = (uint8_t*)outputMatrix;
  
  //Assigning const values
  inputMatrix[0]=const1;
  inputMatrix[5]=const2;
  inputMatrix[10]=const3;
  inputMatrix[15]=const4;
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
  inputMatrix[6]=iv & 0xFFFFFFFF;
  inputMatrix[7]=(iv >> 32) & 0xFFFFFFFF;
  //Counter value in key
  uint64_t keyCounter = 0;

  for (size_t i = 0; i < coreCounter; i++)
  {
    //Assigning C0 and C1
    inputMatrix[8]=keyCounter & 0xFFFFFFFF;
    inputMatrix[9]=(keyCounter >> 32) & 0xFFFFFFFF;
    
    //outputMatrix contains output of core
    salsa20_core_1(outputMatrix,inputMatrix);

    //initializing key for each byte and pointer to outputMatrix
      for (size_t j = 0; j < 64; j++)
      {
        cipher[i*64+j] = msg[i*64+j] ^ *charPointer;
        charPointer++;
      }
      /*
      ============ SIMD APPROACH ============
      */
      //for (size_t simdCounter = 0; simdCounter < 4; simdCounter++)
      //{
      //__m128i simdReg1 = _mm_loadu_si128((__m128i_u *)msg);
      //__m128i simdReg2 = _mm_loadu_si128((__m128i_u *)outputMatrix);

      //}
      /*
      =======================================
      */
      
    keyCounter++;
    charPointer = (uint8_t*)outputMatrix;
  }

  if (restChar != 0)
  {
    inputMatrix[8]=keyCounter & 0xFFFFFFFF;
    inputMatrix[9]=(keyCounter >> 32) & 0xFFFFFFFF;

    salsa20_core_1(outputMatrix,inputMatrix);
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


