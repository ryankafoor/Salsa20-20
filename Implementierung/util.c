#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_INPUT_LENGTH 64

void print_error(const char *func_name, const char *message) {
   if(!message || !func_name) {
        fprintf(stderr, "\033 Error found in function [%s] : %s \033 \n", "print_error", "Invalid input");
        return;
    }
    else {
        fprintf(stderr, "\033 Error found in function [%s] :  %s \033 \n", func_name, message);
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
    printf("\t -B, --benchmark:           Benchmark the implementation, specify the number of iteration directly after -B(without whitespace)  \n");
    printf("\t -k, --key                  (REQUIRED) set the key for salsa20 calculation (32 bytes) \n");
    printf("\t -i, --init_vector          (REQUIRED) set the initialisation vector (8 bytes) \n");
    printf("\t -o, --output_file          (REQUIRED) set the output file path \n");
    printf("\t -h, --help                 Display help page then exit the program\n \n");
    printf("Additionally, the program requires a positional argument that specify the input file path. \n \n");

    //TODO: adjust this part after talking with group (what is the acceptable format for iv and key: hex or decimal, length less than 32/16?)
    printf("Command line example to run the program: \n");
    printf("\t With the improved implementation (0),\n");
    printf("\t With key 0123456789abcdef0123456789abcdef, \n");
    printf("\t With initialisation vector 0123456789abcdef, \n");
    printf("\t To encrypt the input file input.txt, and write the output in output.txt,\n");
    printf("\t By doing 100 iterations: \n\n");
    printf("\t ./main -V 0 -B 100 -k 0123456789abcdef0123456789abcdef -i 0123456789abcdef -o output.txt input.txt\n \n");
    printf(" ==================== END OF SALSA20 HELP PAGE ====================\n");
return;
}


/*
    ================== Handling Hex Input ==================
*/

int is_valid_hex(char c) {
    return isxdigit(c);
}


void hex_to_little_endian_32bit_array(const char* hex, uint32_t* out_array, size_t array_size) {
    size_t hex_length = strlen(hex);
    
    if (hex_length != 64 || array_size < 8 ) {
        fprintf(stderr, "Invalid key: input must be 64 hexadecimal digits\n");
        return;
    }

    for (size_t i = 0; i < hex_length; ++i) {
        if (!is_valid_hex(hex[i])) {
            fprintf(stderr, "Invalid key: input must be in hexadecimal\n");
            return;
        }
    }
    int ret;
    for (size_t i = 0; i < hex_length; i += 8) {
    uint32_t word = 0;
    for (int j = 0; j < 4; j++) { // change 8 to 4, because you read 2 hexadecimal digits (or 1 byte) at a time
        uint8_t byte;
        ret = sscanf(&hex[i + j * 2], "%2hhx", &byte);
        if (ret != 1) {
            fprintf(stderr, "Error reading byte in key: %ld\n", i + j * 2);
            return;
        }
        word |= (uint32_t)byte << (j * 8);
    }
    out_array[i / 8] = word;
    }

    // //uncomment to see the output
    // for(size_t i = 0; i < array_size; i++) {
    //     printf("array[%zu] = 0x%08x\n", i, out_array[i]);
    // }


}



void hex_to_little_endian_uint64(const char* hex, uint64_t iv) {
    size_t hex_length = strlen(hex);
    
   
    if (hex_length != 16) {
        fprintf(stderr, "Invalid nonce: input must be 16 hexadecimal digits\n");
        return;
    }

    // Check that the string only contains hexadecimal digits
    for (size_t i = 0; i < hex_length; ++i) {
        if (!is_valid_hex(hex[i])) {
            fprintf(stderr, "Invalid nonce: input must be in hexadecimal \n");
            return;
        }
    }

    int ret;
    // Convert hex string to uint64_t
    for (size_t i = 0; i < hex_length; i += 2) { //2 hex digits per byte
        uint8_t byte;
        ret = sscanf(&hex[i], "%2hhx", &byte);
        if (ret != 1) {
            fprintf(stderr, "Error reading byte in nonce\n");
            return;
        }
        iv |= (uint64_t)byte << (i * 4); // Shift 8 bits for every bytes
    }

    printf("IV after little endian = 0x%08lx\n", iv);

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


size_t get_file_length(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    size_t length = ftell(fp);

    fclose(fp);
    return length;
}


void pad_hex_string(char* input, char* output, size_t output_length) {
    size_t input_length = strlen(input);
    
    
    if (input_length > output_length) {
        if(output_length == 64) {
            fprintf(stderr, "Invalid key input: key length should be less than %zu hex digits\n", output_length);
            return;
        }
        if (output_length == 16) {
            fprintf(stderr, "Invalid nonce input: nonce length should be less than %zu hex digits\n", output_length);
            return;
        }
    }
    // Check for valid hexadecimal digits
    for (size_t i = 0; i < input_length; ++i) {
        if (!is_valid_hex(input[i])) {
            print_error("util", "input for key / iv should contain only hexadecimal digits \n");
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
    size_t output_length_check = strlen(output);
}