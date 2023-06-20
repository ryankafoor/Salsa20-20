#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


//how many implementations are there?
#define IMPLEMENTATION_MAX 1



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


      if(string = malloc(sb.st_size + 1)){ //st_size does not count the null byte
        perror("Error in allocating memory");
        goto cleanup;
      }

      if(fread(string, 1, sb.st_size, file) != (size_t) sb. st_size){  //fread fails when return value not equal to elements read  
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
            perror("An error occurred while openingthe  file");
            return;
        }

        if(fwrite(string, 1, strlen(string), file) != strlen(string)){
            perror("Error writing to file");
        }

        fclose(file);

   }


   //TODO: Naive C implementation without intrinsics


    void salsa20_core_1(uint32_t output[16], const uint32_t input[16]){

    }


    void salsa20_crypt(size_t mlen, const uint8_t msg[mlen], uint8_t cipher[mlen], uint32_t key[8], uint64_t iv){

    }


   //TODO: C implementation with intrinsics

   //TODO: C and assembly implementation with SIMD (main inplementation)

   //TODO: Benchmarking framework


    //a function to write help message
<<<<<<< HEAD
    //testing
=======
    //test
>>>>>>> c1936a5bb68c2528ac9f06d2d55c5bf69460e0bb
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

}

