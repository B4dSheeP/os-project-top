#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>



typedef struct{
    unsigned delay_ms;
    bool filter_euid:1;
    uid_t euid;
} Options;

Options* get_opts(int argc, char** argv){
    Options* opts = (Options*)malloc(sizeof(Options));
    //filling with default options
    opts->delay_ms = 1000;
    opts->filter_euid = 0;

    for(int i=1; i<argc; i++){
        if(argv[i][0]=='-'){
            
            switch(argv[i][1]){
                case 'd':
                    opts->delay_ms = atoi(argv[i]+2);
                    break;
                case 'u':
                    opts->filter_euid = 1;
                    opts->euid = atoi(argv[i]+2);
                    break;
                default:
                    break;
            }
        }
    }

    return opts;
}


int main(int argc, char** argv){
    Options* options = get_opts(argc, argv);
    printf("%u, %u\n", options->euid, options->delay_ms);
}