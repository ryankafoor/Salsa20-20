#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "intrinsic_impl.c"


//how many implementations are there?
#define IMPLEMENTATION_MAX 1
static const uint32_t const1 = 0x61707865;
static const uint32_t const2 = 0x3320646e;
static const uint32_t const3 = 0x79622d32;
static const uint32_t const4 = 0x6b206574;

char* read_file(const char* path) {
  
  char* string = NULL;
  FILE* file;
  
  if(!(file = fopen(path, "r"))) {
    perror("An error occurred while openingthe  file");
    goto cleanup;
  }

  struct stat sb; //Status buffer to check the status of file


  if(fstat(fileno(file), &sb)==-1){
    perror("Status of file could not be checked");
    goto cleanup;
  }

  if (!S_ISREG(sb.st_mode) || sb.st_size <= 0){
    perror("Error: File not a regular file or invalid size");
    goto cleanup;
  }


  if((string = malloc(sb.st_size + 1))){ //st_size does not count the null byte
    perror("Error in allocating memory");
    goto cleanup;
  }

  if(fread(string, 1, sb.st_size, file) != (size_t) sb. st_size){  //fread failed when return value not equal to elements read  
    perror("Error reading file");
    free(string); 
    string=NULL; 
    goto cleanup; 
  } 
  
  string[sb.st_size] = '\0';  //append null byte at end of string

  cleanup:
      if (file){
        fclose(file);
      }

  return string;
}

   

   
void write_file (const char* path, const char* string){
    
    FILE* file;
    
    if(!(file = fopen(path, "w"))) {
        perror("An error occurred while opening the file");
        return;
    }

    if(fwrite(string, 1, strlen(string), file) != strlen(string)){
        perror("Error writing to file");
    }

    fclose(file);

}



//Rotate function
uint32_t rotate_bits_1(uint32_t number, uint8_t i){
  return (number << i) | (number >> (32 -i));
}

//naive as possible
void transponse_1( uint32_t array[16]){

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


//Naive C implementation without intrinsics


void salsa20_core_1(uint32_t output[16], const uint32_t input[16]){

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


void salsa20_crypt_1(size_t mlen, const uint8_t msg[mlen], uint8_t cipher[mlen], uint32_t key[8], uint64_t iv){
  
  size_t coreCounter = mlen / 64;
  size_t restChar = mlen % 64;

  uint32_t inputMatrix[16];
  uint32_t outputMatrix[16];
  uint32_t *outputPointer = outputMatrix;
  
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
        cipher[j] = msg[j] ^ *outputPointer;
        outputPointer++;
      }    
    keyCounter++;
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
        cipher[j] = msg[j] ^ *outputPointer;
        outputPointer++;
      } 
    }
    
}

uint8_t* test(uint8_t *toEncrypt){
  //These two values we can alter
  uint32_t key[8] = {1,2,3,4,5,6,7,8};
  uint64_t iv = 231;
  //
  size_t mlen = strlen((char*)toEncrypt);

  uint8_t *cipher = malloc(mlen);
  if (!cipher)
  {
    perror("Error allocating memory for Cipher");
    free(cipher);
    EXIT_FAILURE;
  }
  
  salsa20_crypt_1(mlen,toEncrypt,cipher,key,iv);
  
  printf("Message length : %zu\n",mlen);

  for (size_t i = 0; i < mlen; i++)
  {
  printf("Ciphertext : %u\n",cipher[i]);
  }
  return cipher;
}


   //TODO: C implementation with intrinsics

   //TODO: C and assembly implementation with SIMD (main inplementation)

   //TODO: Benchmarking framework


    //a function to write help message
    
    //optional: beautify the help message
void printHelp() {
    printf("TODO: \n");
    printf("Encrypts the input file using the Salsa20 stream cipher\n");
    printf("using the provided key and nonce / initial vector\n");
    printf("");
    printf("  -V, --version=VERSION_NUMBER  use the implementation with the given version number\n");
    printf("  -B, --benchmark[=FILE]        benchmark the implementation and write the results to FILE\n");
    printf("  -k, --key=KEY                 use KEY as the encryption key\n");
    printf("  -i, --init_vector=IV          use IV as the initialisation vector\n");
    printf("  -h, --help                    display this help and exit\n");
}

int main(int argc, char *argv[]) {

  int opt;
  int version_number = 0;

  const char *input_file = NULL;
  const char *output_file = NULL;

  int key = 0;
  int iv = 0;


  //define acceptable options
  static const struct option long_opt[] =
  {
          {"version", required_argument, NULL, 'V'},
          {"benchmark", optional_argument, NULL, 'B'},
          {"key", required_argument, NULL, 'k'},
          {"init_vector", required_argument, NULL, 'i'},
          {"help", no_argument, NULL, 'h'},
          {0, 0, 0, 0}
  };


  while ((opt = getopt_long(argc, argv, "V:B:k:i:h", long_opt, 0)) != -1)
  {
      switch (opt)
      {
        case 'V':
          version_number = atoi(optarg);
          if(version_number < 0 || version_number > IMPLEMENTATION_MAX) {
              printf("Invalid version number. Please see the help page for more information");
              exit(EXIT_FAILURE);
          }
          break;

      case 'h':
              printHelp();
              return EXIT_SUCCESS;
          //TODO: implement printHelp()

      case 'o':
          output_file = optarg;
          break;
      
        case 'k':
          
          break;

      case 'i':
          
          break;
      //TODO: finish cases, add error handling, etc-..
    }

  }
  //msg we can alter
  uint8_t *msg = (uint8_t*)"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  //uint8_t *msg = (uint8_t*)"hello";
  uint8_t *encrypted = test(msg);
  test(encrypted);
  
}

