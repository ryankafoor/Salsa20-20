#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

#define IMPLEMENTATION_MAX 1

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
