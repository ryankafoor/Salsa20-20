CFLAGS = -O0 -march=native -std=gnu17 -fsanitize=address#Options can be added here 
# use -mno-avx if avx should be disabled
WFLAGS = -Wall -Wextra -Wpedantic 

all: main 


main: main.c rotate_bits.S

	$(CC) $(CFLAGS) -o $@ $^  

clean:  
	rm -f main

# Instructions:
# make (makes all) 
# make clean all (removes main and then makes all)
# make CFLAGS="-O3-Wall-Wextra" #Specifies flag 