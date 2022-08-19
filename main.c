#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>


#include "top.h"

typedef enum {
    PRINT,
    KILL,
    RESUME, 
    SUSPEND,
    TERMINATE
} Action;


typedef struct{
    Action act;
    unsigned delay_ms;
    unsigned limit;
    pid_t pid;
    bool filter_euid:1;
    uid_t euid;
} Options;

Options* get_opts(int argc, char** argv){
    Options* opts = (Options*)malloc(sizeof(Options));
    //filling with default options
    opts->delay_ms = 1000000;
    opts->filter_euid = 0;
    opts->pid = 0;
    opts->act = PRINT;
    opts->limit = 0xFFFF;

    if(argc<2) return opts;

    if(!strcmp(argv[1], "kill")) opts->act = KILL;
    else if(!strcmp(argv[1], "resume")) opts->act = RESUME;
    else if(!strcmp(argv[1], "suspend")) opts->act = SUSPEND;
    else if(!strcmp(argv[1], "terminate")) opts->act = TERMINATE;



    for(int i=1; i<argc; i++){
        if(argv[i][0]=='-'){ 
            switch(argv[i][1]){
                case 'd':
                    opts->delay_ms = 1000*atoi(argv[i]+2);
                    break;
                case 'u':
                    opts->filter_euid = 1;
                    opts->euid = atoi(argv[i]+2);
                    //lo implemento solo se mi rimane tempo
                    break;
                case 'p':
                    opts->pid = atoi(argv[i]+2);
                case 'l':
                    opts->limit = atoi(argv[i]+2);
                default:
                    break;
            }
        }
    }

    return opts;
}


int main(int argc, char** argv){
    Options* options = get_opts(argc, argv);
    //printf("%u, %u\n", options->euid, options->delay_ms);
    if(options->act == PRINT){
        //set here a ctrl-c handler for safe exit
        while(1){
            //wipe terminal here
            print_top(options->delay_ms, options->limit);
            struct timespec ts = {.tv_sec=0, .tv_nsec=500000000};
            struct timespec res;
            nanosleep(&ts, &res);

        }
    }
    else{
        int signals[]={0, SIGKILL, SIGCONT, SIGSTOP, SIGTERM};
        kill(options->pid, signals[options->act]);
    }
    free(options);
    exit(EXIT_SUCCESS);
}