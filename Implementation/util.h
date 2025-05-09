#ifndef __UTIL_H__
#define __UTIL_H__


// function to print error message to stderr with errno message if available
void print_error(const char *func_name, const char *message);

// function to print help message
void print_help();

//function to check if a character is a valid hex character
int is_valid_hex(char c);

//function to convert a hex character to decimal
uint32_t hex_to_decimal(char c);

//function to convert a hex string to a 32 bit little endian array FOR KEYS!!!
void hex_array(const char* hex, uint32_t* out_array, size_t array_size); 

//function to convert a hex string to a 64 bit little endian uint64 FOR IV / NONCE!!!
uint64_t hex_uint64(const char* hex);


#endif // __UTIL_H__
