#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include "intrinsic_impl.c"
#include <time.h>
#include "util.h"
#include "util.c"
//#include <malloc.h>





// aligned_malloc(1000, 128) will return a memory address
// that is a multiple of 128 and that points to memory of size 1000 bytes.
//offset_t is a uint16_t, supports up to 64KB alignment, a size which is already unlikely to be used for alignment.
typedef uint16_t offset_t;
#define PTR_OFFSET_SZ sizeof(offset_t)

void* aligned_malloc(size_t required_bytes, size_t alignment){
  int offset = alignment - 1;
  void* p_addr;
  // we also need about 2 bytes for storing offset to get to orig malloc address during aligned_free().
  uint32_t hdr_size = PTR_OFFSET_SZ + (alignment - 1);
  if((p_addr = (void * ) malloc(required_bytes + hdr_size)) == NULL){
  return NULL;
  }
  // a) bit-shift to move address to aligned_addr
  //    Note that this operates on powers of two
  //void* aligned_addr = (void * ) (((size_t)(p_addr) + offset) & ~(offset));
  //b) OR use modulo operator to get how much to move forward
  int move_forward = (alignment - ((size_t)p_addr % alignment));
  void* aligned_addr= (void*)((size_t)p_addr + move_forward);
  // store 16-bit offset instead of a 32bit or 64 bit platform address.
  *((size_t *) aligned_addr - 1) = (size_t)(aligned_addr - p_addr);
  return aligned_addr;
}

void aligned_free(void *aligned_addr ){
  /* Find the address stored by aligned_malloc() ,"size_t" bytes above the current pointer then free it using free() API.*/
  size_t offset = *((size_t *) aligned_addr);
  // get to p_addr using offset and aligned_addr value and call 
  
  // free() on it.
  free((void *)(*((size_t*) aligned_addr) - offset));
}






//how many implementations are there?
#define IMPLEMENTATION_MAX 1
#define ITERATION_MAX 10000
#define IV_SIZE 8
#define KEY_SIZE 16

//constants read as expand 32-byte k, written in 4 bytes of little endian
static const uint32_t const1 = 0x61707865; //apxe
static const uint32_t const2 = 0x3320646e; //3 dn
static const uint32_t const3 = 0x79622d32; //yb-2
static const uint32_t const4 = 0x6b206574; //k et


static char* read_file(const char* path) {
  char* string = NULL;
  FILE* file;
  
  if(!(file = fopen(path, "r")))  {
    perror("An error occurred while opening the file");
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


  if((string = aligned_malloc(sb.st_size + 1,32)) == NULL){ //st_size does not count the null byte
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
        if((fclose(file)) == EOF) { 
          perror("Error closing file");
          exit(EXIT_FAILURE);
        }
      }
  
  return string;
}


//Test Implementation for alligned read file. Do not Modify!!
/*

static char* read_file(const char* path) {
    char* string = NULL;
    FILE* file = NULL;
    struct stat sb;

    if (!(file = fopen(path, "r"))) {
        perror("An error occurred while opening the file");
        goto cleanup;
    }

    if (fstat(fileno(file), &sb) == -1) {
        perror("Status of file could not be checked");
        goto cleanup;
    }

    if (!S_ISREG(sb.st_mode) || sb.st_size <= 0) {
        perror("Error: File not a regular file or invalid size");
        goto cleanup;
    }

    // Calculate the required size for alignment
    size_t required_size = sb.st_size + 1;

    // Allocate memory with alignment
    if (posix_memalign((void**)&string, 32, required_size) != 0) {
        perror("Error in allocating aligned memory");
        goto cleanup;
    }

    if (fread(string, 1, sb.st_size, file) != (size_t)sb.st_size) {
        perror("Error reading file");
        free(string);
        string = NULL;
        goto cleanup;
    }

    string[sb.st_size] = '\0';

cleanup:
    if (file) {
        if (fclose(file) == EOF) {
            perror("Error closing file");
            exit(EXIT_FAILURE);
        }
    }

    return string;
}
   
*/

//posix_alignment? why not?
   
static void write_file (const char* path, const char* string){
    
    FILE* file;
    
    if(!(file = fopen(path, "w"))) {
        perror("An error occurred while opening the file");
        exit(EXIT_FAILURE);
    }

    if(fwrite(string, 1, strlen(string), file) != strlen(string)){
        perror("Error writing to file");
        exit(EXIT_FAILURE);
    }

    if((fclose(file)) == EOF) {
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }

}



//Rotate function
//__attribute__((hot))
//__attribute__((always_inline))
static uint32_t rotate_bits_1(uint32_t number, uint8_t i){
  return (number << i) | (number >> (32 -i));
}

//naive as possible

static void transpose_1( uint32_t array[16]){

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


static void salsa20_core_v1(uint32_t output[16], const uint32_t input[16]){

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
  
    transpose_1(output);
  }

  for (uint8_t i = 0; i < 16; i++)
  {
    output[i] += input[i]; 
  }
  
  

}


static void salsa20_crypt_v1(size_t mlen, const uint8_t msg[mlen], uint8_t cipher[mlen], uint32_t key[8], uint64_t iv){
  
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
    salsa20_core_v1(outputMatrix,inputMatrix);

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

    salsa20_core_v1(outputMatrix,inputMatrix);
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



static uint8_t* test(uint8_t *toEncrypt, size_t mlen){
  //These two values we can alter
  uint32_t key[8] = {1,2,3,4,5,6,7,8};
  uint64_t iv = 231;
  //
  uint8_t *cipher = aligned_malloc(mlen*sizeof(uint8_t),32);
  if (cipher==NULL)
  {
    perror("Error allocating memory for Cipher: test(uint8_t *toEncrypt, size_t mlen)");
    exit(EXIT_FAILURE);
  }
  if (mlen < 256)
  {
    salsa20_crypt_v1(mlen,toEncrypt,cipher,key,iv);
  }
  
  salsa20_crypt(mlen,toEncrypt,cipher,key,iv);

  return cipher;
}



int main(int argc, char *argv[]) {
  int opt;
  const char *input_text = NULL;
  const char *output_file = NULL;

  uint32_t key[8];
  uint64_t iv = 0;

  //temporary storage to store the key and iv input (from user), before padding it with zeroes
  char input_key[65];
  char input_iv[17];

  int version_number = 0;
  int benchmark_iteration = 1;

  int benchmark_flag = 0;
  int key_flag = 0;
  int iv_flag = 0;
  int output_flag = 0;
  int debug_flag = 0;

  double avg_time;    //for benchmarking
  clock_t start, end;

  size_t mlen = 0;

  //define acceptable options
  static const struct option long_opt[] =
  {
          {"debug", no_argument, NULL, 'd'},
          {"version", required_argument, NULL, 'V'},
          {"benchmark", optional_argument, NULL, 'B'},
          {"key", required_argument, NULL, 'k'},
          {"init_vector", required_argument, NULL, 'i'},
          {"output", required_argument, NULL, 'o'},
          {"help", no_argument, NULL, 'h'},
          {0, 0, 0, 0}
  };


  while ((opt = getopt_long(argc, argv, "V:B:k:i:o:h:d", long_opt, 0)) != -1)
  {
      switch (opt)
      {
        case 'd':
          debug_flag = 1;
          //TODO: debug mode? print out more information? e.g. state of variables / key / iv / etc.
          break;
        case 'V':
          version_number = atoi(optarg);
          printf("Version number: %d\n", version_number);
          if(version_number < 0 || version_number > IMPLEMENTATION_MAX) {
              printf("Invalid version number. Please see the help page for more information");
              exit(EXIT_FAILURE);
          }
          break;
      case 'h':
          print_help();
          return EXIT_SUCCESS;
      case 'o':
          output_file = optarg;
          output_flag = 1;
          break;
      case 'k':
          pad_hex_string(optarg, input_key, 64);
          hex_to_little_endian_32bit_array(input_key, key, 8);
          key_flag = 1;
          break;
      case 'i':
          pad_hex_string(optarg, input_iv, 16);
          printf("IV: %s\n", input_iv);
          hex_to_little_endian_uint64(input_iv, &iv);
          iv_flag = 1;
          break;
      case 'B':
          benchmark_flag = 1;
          benchmark_iteration = strtoull(optarg, NULL, 10);
          if(!benchmark_iteration || benchmark_iteration > ITERATION_MAX){
            print_error("main", "Benchmark mode: invalid number of iterations. See help page with -h for more information. \n");
            exit(EXIT_FAILURE);
          }
          break;
      default:
          print_error("main", "Invalid program argument. See help page with  -h for more information. \n");
          exit(EXIT_FAILURE);
    }
  }

  if (argv[optind] == NULL) {
    printf("Input file missing");
    exit(EXIT_FAILURE);
  }

  input_text = read_file(argv[optind]);
  printf("Input file: %s\n", argv[optind]);
  if (input_text == NULL) {
    printf("Input file invalid");
    exit(EXIT_FAILURE);
  }

  optind++;
  if (optind < argc) {
    printf("Too many arguments");
    exit(EXIT_FAILURE);
  }

  if(!key_flag){
    printf("Key missing, see help page with -h for more information");
    exit(EXIT_FAILURE);
  }
  if(!iv_flag){
    printf("Initialisation vector missing, see help page with -h for more information");
    exit(EXIT_FAILURE);
  }
  if(!output_flag){
    printf("Output file missing, see help page with -h for more information");
    exit(EXIT_FAILURE);
  }

  //handle input file: extract message and message length, update mlen
  //Input file should be valid and required arguments are provided

  mlen = strlen(input_text);
  if (mlen == 0) {
    printf("Input file is empty, nothing to encrypt/decrypt");
    exit(EXIT_FAILURE);
  }

  uint8_t *cipher = aligned_malloc(mlen*sizeof(uint8_t),32);
  if (cipher==NULL)
  {
    perror("Error allocating memory for Cipher: test(uint8_t *toEncrypt, size_t mlen)");
    exit(EXIT_FAILURE);
  }



  if(benchmark_flag){
    printf("Benchmarking mode using %d iterations \n ", benchmark_iteration);
    printf("Implementation version: %d \n", version_number);
    start = clock();
  }
  else{
    printf("Implementation version: %d \n", version_number);
  }
  

  /*
  At this point, all inputs should be correct, and input file should be readable.
  The following variable should hold the correct values:

  - input_text contains the message to be encrypted/decrypted
  - mlen contains the length of the message
  - key contains the key in little endian format
  - iv contains the initialisation vector / nonce in little endian format
  - cipher is a pointer to the output buffer, what we want to write to the output file
  */
  switch(version_number) {
    case 0:
      printf("Version 0 selected, encryption in process\n");

      for(size_t i = 0; i < benchmark_iteration - 1; i++) {
      salsa20_crypt(mlen, input_text, cipher, key, iv);
      }

      write_file(output_file,(char *)cipher);
      break;
    case 1:
      printf("Version 1 selected, encryption in process\n");

      for(size_t i = 0; i < benchmark_iteration - 1; i++) {
      salsa20_crypt_v1(mlen, input_text, cipher, key, iv);
      }

      write_file(output_file,(char *)cipher);
      break;
    default:
      printf("Version number is invalid.");
      exit(EXIT_FAILURE);
    }

  if(benchmark_flag){
    end = clock();
    avg_time = (((double) (end - start)) / CLOCKS_PER_SEC) / benchmark_iteration;
    printf("\nBenchmarking mode finished with %zu iterations\n", benchmark_iteration);
    printf("Average time taken: %f seconds \n", avg_time);
  }

  //for debug: run the program on output
  uint8_t *decrypted_text = aligned_malloc(mlen*sizeof(uint8_t),32);
  if (decrypted_text == NULL) {
    perror("Error allocating memory for decrypted_text: main");
    exit(EXIT_FAILURE);
  }

  //print debug info: checks if running the program with the output will result in the original text
  if(debug_flag){
    char* encrypted_text = read_file(output_file);
    if (encrypted_text == NULL) {
      printf("Output file invalid");
      exit(EXIT_FAILURE);
    }


    printf("Debug mode: checking if running the program with the output will result in the original text\n");
    switch(version_number) {
      case 0:
        salsa20_crypt(mlen, encrypted_text, decrypted_text, key, iv);
        break;
      case 1:
        salsa20_crypt_v1(mlen, input_text, decrypted_text, key, iv);
        break;
      default:
        printf("Version number is invalid.");
        exit(EXIT_FAILURE);
    }
    if(memcmp(input_text, decrypted_text, mlen) != 0) {
      printf("Decryption failed.\n");
      return EXIT_FAILURE;
    }
  printf("Decryption succeeded.\n");
  }

  aligned_free(cipher);
  aligned_free(decrypted_text);

  printf("Program finished successfully.\n");

return EXIT_SUCCESS;

  /*
  ==========================================
   TEST CODE in main
  ==========================================
  */

  //msg we can alter
  //uint8_t *msg = (uint8_t*)"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzABCDE";
  //uint8_t *msg = (uint8_t*)"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.156464";
  
  char* msg = read_file("input.txt");
  if(!msg){
    printf("Failure in reading from file\n");
    return EXIT_FAILURE;
  } 
  size_t msgLen = strlen((char*)msg);

  /*
  printf("Original Message :\n");
  for (size_t i = 0; i < msgLen; i++)
  {
  printf("index %zu: %u\n",i ,msg[i]);
  }
  
  */
  printf("\n");
  //check

  struct timespec start_time;
  struct timespec end_time;

  clock_gettime(CLOCK_MONOTONIC,&start_time);

  uint8_t *encrypted = test((uint8_t *)msg,msgLen);

  clock_gettime(CLOCK_MONOTONIC, &end_time);

  double time = end_time.tv_sec - start_time.tv_sec + 1e-9*(end_time.tv_nsec - start_time.tv_nsec);
  double time_for_one_byte = time / msgLen;
/*
  printf("Encrypted Message :\n");
  for (size_t i = 0; i < msgLen; i++)
  {
  printf("index %zu: %u\n",i ,encrypted[i]);
  }
  printf("\n");
  // check if the decrypted message matches the original message
  */
  uint8_t *decrypted = test(encrypted,msgLen);

  write_file("output.txt",(char *)decrypted);
  /*
  printf("Decrypted Message :\n");
  for (size_t i = 0; i < msgLen; i++)
  {
  printf("index %zu: %u\n",i ,decrypted[i]);
  }
  printf("\n");

*/
  if(memcmp(msg, decrypted, msgLen) != 0) {
      printf("Decryption failed.\n");
      return EXIT_FAILURE;
    }
  printf("Decryption succeeded.\n");
   
  aligned_free(encrypted);
  aligned_free(decrypted);
  

  printf("Time taken for ecrypting message: %f\n", time);
  printf("Average time for one byte: %f\n", time_for_one_byte);
  
  /*
  ==========================================
   TEST CODE in main END
  ==========================================
  */

  //Run with ./main -k 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef -o output.txt -i 0123456789abcdef test.txt
  

  return EXIT_SUCCESS;
}

