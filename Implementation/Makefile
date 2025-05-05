.PHONY: all clean input_1 input_2 input_3 input_4 input_custom

CFLAGS = -O0 -march=native -std=gnu17 #Options can be added here 
# use -mno-avx if avx should be disabled
WFLAGS = -Wall -Wextra -Wpedantic 

CUSTOM_NUM = 0

all: main 


main: main.c rotate_bits.S

	$(CC) $(CFLAGS) -o $@ $^  

clean:  
	rm -f main

input_1:
	base64 /dev/urandom | head -c 1000000 > input.txt

input_2:
	base64 /dev/urandom | head -c 10000000 > input.txt

input_3:
	base64 /dev/urandom | head -c 100000000 > input.txt

input_4:
	base64 /dev/urandom | head -c 200000000 > input.txt

input_custom:
	base64 /dev/urandom | head -c $(CUSTOM_NUM) > input.txt


# Instructions:
# make (makes all) 
# make clean all (removes main and then makes all)
# make CFLAGS="-O3-Wall-Wextra" #Specifies flag 
# make input_custom CUSTOM_NUM="Some Number" # Generates an input file with custom number of chars 