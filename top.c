#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <assert.h>

#include "top.h"

int dirent_cmp_on_name(const struct dirent** a, const struct dirent** b){
    return strcmp((*a)->d_name, (*b)->d_name);
}

int is_digit(const struct dirent* a){
    size_t len = strlen((a)->d_name);
    char r = 0 ;
    for(int i = 0; !r && i < len ; i++){
        r |= a->d_name[i] < 0x30;
        r |= a->d_name[i] > 0x39;
    }
    return !r;
}

void proc_stat(Process* process){
    char filename[20];
    sprintf(filename, "/proc/%d/stat", process->pid);
    FILE *f = fopen(filename, "r");

    assert(fscanf(f, "%*d %*s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %ld %ld %*d %*d %*u %lu",
                                                         &process->s, 
                                                         &process->utime, 
                                                         &process->stime,
                                                         &process->pr,
                                                         &process->ni, 
                                                         &process->virt
                                                         ) == 6 && "error in parsing stat file");
    fclose(f);
}

void proc_statm(Process* process){
    strcpy(process->user, "fuffa_user");
    process->res = process->shr = 0;
    process->cpu_percentage = process->mem_percentage = 0;
    strcpy(process->command, "fuffa_command");
}

void print_top(){
    //task info
    //cpu inf
    //mem inf
    struct dirent **dir_entries;
    int res = scandir("/proc/", &dir_entries, is_digit, dirent_cmp_on_name);
    Process **processes = (Process**)malloc(sizeof(Process**)*res);
    for(int i = 0; i < res; i++){
        Process *proc = (Process*)malloc(sizeof(Process)); 
        proc->pid = atoi((*(dir_entries+i))->d_name);
        proc_stat(proc);
        proc_statm(proc);
        processes[i] = proc;
    }

    printf("PID USER PR NI VIRT RES SHR S %%CPU %%MEM TIME+ COMMAND\n");
    for(int i = 0; i<res; i++){
        printf("%d %s %ld %ld %lu %u %u %c %.1f %.1f %lu %s\n",
                                                processes[i]->pid,
                                                processes[i]->user,
                                                processes[i]->pr,
                                                processes[i]->ni,
                                                processes[i]->virt/1000, //virt è in bytes
                                                processes[i]->res,
                                                processes[i]->shr,
                                                processes[i]->s,
                                                processes[i]->cpu_percentage,
                                                processes[i]->mem_percentage,
                                                processes[i]->utime+processes[i]->stime,
                                                processes[i]->command
                                                );
    }

    //free heap memory
    for(int i = 0; i<res; i++) free(processes[i]);
    free(processes);
    
}

inline void wipe_terminal(){ write(STDOUT_FILENO, "\x1b\x5b\x48\x1b\x5b\x32\x4a\x1b\x5b\x33\x4a", 11); }