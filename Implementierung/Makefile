CFLAGS = -O0 -march=native #Options can be added here 
# use -mno-avx if avx should be disabled

all: main 


main: main.c rotate_bits.S

	$(CC) $(CFLAGS) -o $@ $^

clean:  
	rm -f main

# Instructions:
# make (makes all)
# make main 
# make clean all (cleans and then makes all)
# make CFLAGS="-O3-Wall-Wextra" #Specifies flag 