#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <error.h>
#include <stdbool.h>
#include <fcntl.h>

#include "top.h"

int dirent_cmp_on_name(const struct dirent** a, const struct dirent** b){
    return strcmp((*a)->d_name, (*b)->d_name);
}


int cmp_on_cpu_usage(const void* a, const void* b){
    const Process** ap = a;
    const Process** bp = b;
    return  ((*bp)->cpu_percentage*100)-((*ap)->cpu_percentage*100);
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

Cpu* get_cpu(){
    Cpu* cpu = (Cpu*)malloc(sizeof(Cpu));
    FILE* f = fopen("/proc/stat", "r");

    assert(fscanf(f, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
                                    &cpu->user_time, 
                                    &cpu->userlow_time,
                                    &cpu->sys_time, 
                                    &cpu->idle_time,
                                    &cpu->iowait_time,
                                    &cpu->irq_time,
                                    &cpu->softirq_time,
                                    &cpu->steal_time,
                                    &cpu->guest_time,
                                    &cpu->guestnice_time) 
                                    == 10);
    
    fclose(f);
    return cpu;
}

bool proc_stat(Process* process){
    char filename[20];
    sprintf(filename, "/proc/%d/stat", process->pid);
    FILE *f = fopen(filename, "r");
    if(!f) return false;
    assert(fscanf(f, "%*d %s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %ld %ld %*d %*d %llu",
                                                         process->command,
                                                         &process->s, 
                                                         &process->utime, 
                                                         &process->stime,
                                                         &process->pr,
                                                         &process->ni,
                                                         &process->starttime 
                                                         ) && "error in parsing stat file");
    
    fclose(f);
    return true;
}

bool proc_loginuid(Process* process){
    char filename[20];
    sprintf(filename, "/proc/%d/loginuid", process->pid);
    FILE *f = fopen(filename, "r");
    if(!f) return false;
    int res;
    assert(fscanf(f, "%d", &res) == 1 && "error in parsing loginuid file");
    fclose(f);
    process->user = res;
    return true;
}

bool proc_statm(Process* process){
    char filename[20];
    sprintf(filename, "/proc/%d/statm", process->pid);
    FILE *f = fopen(filename, "r");
    if(!f) return false;
    assert(fscanf(f, "%lu %u %u", &process->virt, &process->res, &process->shr) == 3 
                                                && "error in parsing statm file");
    
    
    process->mem_percentage = 0;
    fclose(f);
    return true;
}

float uptime(){
    FILE *f = fopen("/proc/uptime", "r");
    if(!f) error("error in reading /proc/uptime");
    float res;
    assert(fscanf(f, "%f", &res) == 1 && "error in reading /proc/uptime");
    return res;
}

void print_top(unsigned delay_ms, unsigned limit){
    //task info
    //cpu inf
    //mem inf
    struct dirent **dir_entries;
    int res = scandir("/proc/", &dir_entries, is_digit, dirent_cmp_on_name);
    Process **processes = (Process**)malloc(sizeof(Process**)*res);

    Cpu* cpu = get_cpu();
    float system_uptime = uptime();
    
    
    for(int i = 0; i < res; i++){
        Process *proc = (Process*)malloc(sizeof(Process)); 
        proc->pid = atoi((*(dir_entries+i))->d_name);
        proc_stat(proc);
        proc_statm(proc);
        proc_loginuid(proc);
        proc->cpu_percentage = (proc->utime+proc->stime) / (system_uptime - proc->starttime/100);
        processes[i] = proc;
    }
    
    
    
    qsort(processes, res, sizeof(Process*), cmp_on_cpu_usage);

    wipe_terminal();
    printf("%7s %7s %4s %4s %7s %7s %7s %2s %7s %7s %7s %7s\n", "PID", "USER", "PR", "NI", "VIRT", "RES", "SHR", "S", "%CPU", "%MEM", "TIME+", "COMMAND");
    for(int i = 0; i<res && i < limit; i++){
        printf("%7d %7d %4ld %4ld %7lu %7u %7u %2c %7.1f %7.1f %7lu %7s\n",
                                                processes[i]->pid,
                                                processes[i]->user,
                                                processes[i]->pr,
                                                processes[i]->ni,
                                                processes[i]->virt/(1<<10), //virt è in bytes
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
    free(cpu);
    
}

inline void wipe_terminal(){ write(STDOUT_FILENO, "\x1b\x5b\x48\x1b\x5b\x32\x4a\x1b\x5b\x33\x4a", 11); }