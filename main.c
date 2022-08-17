#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>

enum Action{
    PRINT,
    KILL,
    RESUME, 
    SUSPEND,
    TERMINATE
};


typedef struct{
    Action act;
    unsigned delay_ms;
    pid_t pid;
    bool filter_euid:1;
    uid_t euid;
} Options;

Options* get_opts(int argc, char** argv){
    Options* opts = (Options*)malloc(sizeof(Options));
    //filling with default options
    opts->delay_ms = 1000;
    opts->filter_euid = 0;
    opts->pid = 0;
    opts->act = PRINT;


    if(!strcmp(argv[1], "kill")) opts->act = KILL;
    if(!strcmp(argv[1], "resume")) opts->act = RESUME;
    if(!strcmp(argv[1], "suspend")) opts->act = SUSPEND;
    if(!strcmp(argv[1], "terminate")) opts->act = TERMINATE;



    for(int i=2; i<argc; i++){
        if(argv[i][0]=='-'){
            
            switch(argv[i][1]){
                case 'd':
                    opts->delay_ms = atoi(argv[i]+2);
                    break;
                case 'u':
                    opts->filter_euid = 1;
                    opts->euid = atoi(argv[i]+2);
                    break;
                case 'p':
                    opts->pid = atoi(argv[i]+2);
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
    if(options->act == PRINT){
            while(1){
                //wipe terminal here
                print_top();
                //delay here
            }
    }
    else if(options->act == KILL){}
    else if(options->act == RESUME){}
    else if(options->act == SUSPEND){}
    else if(options->act == TERMINATE){}
    exit(EXIT_SUCCESS);
}