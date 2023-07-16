#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <byteswap.h>

#define MAX_INPUT_LENGTH 64

int is_positive_number(const char *str) {
    for (int i = 0; str[i] != '\0'; ++i) {
        if (!isdigit(str[i])) {
            if(str[i] == '&'){
            print_error("main", "Benchmark mode cannot accept & as an argument. Please see help page with ./main -h. \n");
            exit(EXIT_FAILURE);
            }
            return 0;
        }
    }
    return 1;
}

void print_error(const char *func_name, const char *message) {
   if(!message || !func_name) {
        fprintf(stderr, "Error found in function [%s]. %s \n", "print_error", "Invalid input");
        return;
    }
    else {
        fprintf(stderr, "Error found in function [%s]. %s \n", func_name, message);
        return;
    }
}

void print_help() {
    printf(" ======================  SALSA20 HELP PAGE  ======================\n \n");
    printf("The function is a symmetric cipher that takes a key and an initializing vector\n");
    printf("from user input, then use them to create a series of key cipher that will be used\n");
    printf("to encrypt and decrypt message inside the input file, then write the result to the output file.\n \n \n");

    printf("Available options / arguments are:\n");
    printf("\t -V, --version:             Set the implementation version (0 or 1) \n");
    printf("\t -B, --benchmark:           Benchmark the program by doing iterations based on specified argument \n");
    printf("\t                            If no argument is specified, then the program will run once\n");
    printf("\t -k, --key                  (REQUIRED) set the key for salsa20 calculation (32 bytes of hexadecimal) \n");
    printf("\t -i, --init_vector          (REQUIRED) set the initialisation vector (8 bytes of hexadecimal) \n");
    printf("\t -o, --output_file          (REQUIRED) set the output file path \n");
    printf("\t -h, --help                 Display help page then exit the program\n");
    printf("\t -d, --debug                Enter debug mode to check the correctness of both implementation\n\n");
    printf("Additionally, the program requires a positional argument that specify the input file path. \n \n");
    printf("All options with argument accept both argument with or without whitespace after the -option,\n");
    printf("meaning the program accepts both -B100 and -B 100.\n");
    printf("Hexadecimal arguments do not require the prefix 0x. Simply put e.g. -i 0123abcde.\n\n");

    printf("Furthermore a script to generate a text file is provided.\n");
    printf("As an example to generate a text file with 2 millions characters, we could run:\n\n");
    printf("\t make input_custom CUSTOM_NUM=\"2000000\"\n\n");
    printf("The generated text file will have the name input.txt\n");

    printf("\n");
    printf("Command line example to run the program: \n");
    printf("\t With the improved implementation (0),\n");
    printf("\t With key 0123456789abcdef0123456789abcdef, \n");
    printf("\t With initialisation vector 0123456789abcdef, \n");
    printf("\t To encrypt the input file input.txt, and write the output in output.txt,\n");
    printf("\t By doing 100 iterations: \n\n");
    printf("\t ./main -V 0 -B 5 -k 0123456789abcdef0123456789abcdef -i 0123456789abcdef -o output.txt input.txt\n \n");
    printf(" ==================== END OF SALSA20 HELP PAGE ====================\n");
return;
}


/*
    ================== Handling Hex Input ==================
*/
//see util.h for more information
int is_valid_hex(char c) {
    return isxdigit(c);
}


void hex_array(const char* hex, uint32_t* out_array, size_t array_size) {
    size_t hex_length = strlen(hex);
    
    if (hex_length != 64 || array_size < 8 ) {
        fprintf(stderr, "Invalid key: input must be 64 hexadecimal digits\n");
        exit(EXIT_FAILURE);
        return;
    }

    for (size_t i = 0; i < hex_length; ++i) {
        if (!is_valid_hex(hex[i])) {
            fprintf(stderr, "Invalid key: input must be in hexadecimal\n");
            exit(EXIT_FAILURE);
            return;
        }
    }
    for (size_t i = 0; i < array_size; i++) {
        char hexGroup[9];  
        memcpy(hexGroup, hex + (i * 8), 8);
        hexGroup[8] = '\0';

        out_array[array_size - 1 - i] = strtoul(hexGroup, NULL, 16);
    }
}



uint64_t hex_uint64(const char* hex) {
    size_t hex_length = strlen(hex);
    uint64_t iv;
   
    if (hex_length != 16) {
        fprintf(stderr, "Invalid nonce /iv: input must be 16 hexadecimal digits\n");
        exit(EXIT_FAILURE);
    }

    // Check that the string only contains hexadecimal digits
    for (size_t i = 0; i < hex_length; ++i) {
        if (!is_valid_hex(hex[i])) {
            fprintf(stderr, "Invalid nonce / iv: input must be in hexadecimal \n");
            exit(EXIT_FAILURE);
        }
    }

    iv = strtoul(hex, NULL, 16);
    return iv;
}




uint32_t hex_to_decimal(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else
        return 0;
}


/*/
    ================== ^^^^ Handling Hex Input ^^^^ ==================
*/




void pad_hex_string(char* input, char* output, size_t output_length) {
    size_t input_length = strlen(input);
    
    if (input_length > output_length) {
        if(output_length == 64) {
            fprintf(stderr, "Invalid key input: key length should be less than %zu hex digits\n", output_length);
            exit(EXIT_FAILURE);
            return;
        }
        if (output_length == 16) {
            fprintf(stderr, "Invalid nonce /iv: nonce length should be less than %zu hex digits\n", output_length);
            exit(EXIT_FAILURE);
            return;
        }
    }
    // Check for valid hexadecimal digits
    for (size_t i = 0; i < input_length; ++i) {
        if (!is_valid_hex(input[i])) {
            print_error("utility", "Input for key / iv is required and should contain only hexadecimal digits. \n");
            exit(EXIT_FAILURE);
            return;
        }
    }
    
    size_t padding_length = output_length - input_length;
    for (size_t i = 0; i < padding_length; ++i) {
        output[i] = '0';
    }
    
    for (size_t i = 0; i < input_length; ++i) {
        output[i + padding_length] = input[i];
    }
    
    // Null terminate the output string
    output[output_length] = '\0';
}