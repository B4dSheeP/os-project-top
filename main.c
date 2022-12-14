#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
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
    opts->delay_ms = 1000;
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
                    opts->delay_ms = atoi(argv[i]+2);
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
    
    if(options->act == PRINT){
        struct sigaction sa = {0};
        sa.sa_handler = ctrlc_handler;
        sigaction(SIGINT, &sa, NULL);
        while(print_top(options->limit)){
            llu_int delay_ns = options->delay_ms*1e6;
            struct timespec ts = {.tv_sec=delay_ns/1000000000LL, .tv_nsec=delay_ns%1000000000LL}, res;
            nanosleep(&ts, &res);
        }
        printf("GOODBYE! :-)\n");
    }
    else{
        int signals[]={0, SIGKILL, SIGCONT, SIGSTOP, SIGTERM};
        kill(options->pid, signals[options->act]);
    }
    free(options);
    exit(EXIT_SUCCESS);
}