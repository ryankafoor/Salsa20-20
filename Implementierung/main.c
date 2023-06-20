#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define IMPLEMENTATION_MAX 1


   //TODO: Open and read from file and return pointer


   
   //TODO: Write to a file 




   //TODO: Naive C implementation without intrinsics


    void salsa20_core_1(uint32_t output[16], const uint32_t input[16]){
     
     

    }


    void salsa20_crypt(size_t mlen, const uint8_t msg[mlen], uint8_t cipher[mlen], uint32_t key[8], uint64_t iv){

       
    }


   //TODO: C implementation with intrinsics

   //TODO: C and assembly implementation with SIMD (main inplementation)

   //TODO: Benchmarking framework



   int main(int argc, char *argv[]) {

    int opt;
    int version_number = 0;

    char *input_file = NULL;
    char *output_file = NULL;

    int key = 0;
    int iv = 0;


    //define acceptable options
     static struct option long_opt[] =
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
                print_error("main", "Invalid version number. Please see the help page for more information");
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
