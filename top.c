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
#include <signal.h>

#include "top.h"


static char exit_cond = 0;

int dirent_cmp_on_name(const struct dirent** a, const struct dirent** b){
    return strcmp((*a)->d_name, (*b)->d_name);
}


int cmp_on_cpu_usage(const void* a, const void* b){
    const Process** ap = a;
    const Process** bp = b;
    return  ((*bp)->cpu_percentage*100)-((*ap)->cpu_percentage*100);
}

int index_in_mem(const char* buf){
    char* fields[]={
        "MemTotal",
        "MemFree",
        "MemAvailable",
        "Buffers",
        "Cached",
        "SwapTotal",
        "SwapFree",
        "SwapCached"
    };
    for(int i=0; i<8; i++)
        if(!strcmp(buf, fields[i])) return i;
    return -1;
}

llu_int cpu_total(Cpu* cpu){
    llu_int* cpu_as_array = (llu_int*)cpu;
    llu_int r = 0;
    for(int i=0; i<10; i++) r += cpu_as_array[i];
    return r;
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

Mem* get_mem(){
    Mem* mem = (Mem*)malloc(sizeof(Mem));
    unsigned long *mem_as_array = (unsigned long *)mem;
    FILE* f = fopen("/proc/meminfo", "r");
    char buf[100];
    while (fgets(buf, sizeof(buf), f) != NULL) {
		char *c = strchr(buf, ':');
		if (!c) continue;
		*c = '\0';
		int i = index_in_mem(buf); //questa e' una delle mie peggiori zozzate
		if (i >= 0) mem_as_array[i] = strtoul(c+1, NULL, 10);
	}

    fclose(f);
    return mem;

}

bool proc_stat(Process* process){
    char filename[20];
    char command[200];
    sprintf(filename, "/proc/%d/stat", process->pid);
    FILE *f = fopen(filename, "r");
    if(!f) return false;
    assert(fscanf(f, "%*d %s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %ld %ld %*d %*d %llu",
                                                         command,
                                                         &process->s, 
                                                         &process->utime, 
                                                         &process->stime,
                                                         &process->pr,
                                                         &process->ni,
                                                         &process->starttime 
                                                         ) && "error in parsing stat file");
    int si=0;
    while(command[si++]!='(');
    strcpy(process->command, command+si);
    int ei=strlen(process->command);
    while(process->command[ei--]!=')');
    process->command[ei+1]='\0';
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
    fclose(f);
    return res;
}

bool print_top(unsigned limit){
    if(exit_cond) return 0;
    struct dirent **dir_entries;
    int res = scandir("/proc/", &dir_entries, is_digit, dirent_cmp_on_name);
    Process **processes = (Process**)malloc(sizeof(Process**)*res);

    Cpu* cpu = get_cpu();
    Mem* mem = get_mem();
    float system_uptime = uptime();
    unsigned long used_mem = mem->total-mem->free-mem->buffers-mem->cached;
    int tasks[5]={0,0,0,0,0};
    
    for(int i = 0; i < res; i++){
        Process *proc = (Process*)malloc(sizeof(Process)); 
        proc->pid = atoi((*(dir_entries+i))->d_name);
        proc_stat(proc);
        proc_statm(proc);
        proc_loginuid(proc);
        proc->cpu_percentage = (proc->utime+proc->stime) / (system_uptime - proc->starttime/100);
        proc->mem_percentage = (proc->res+proc->shr)*100 / (float)used_mem;
        for(int j=0; j<4; j++)if(proc->s==STATUSES[j])tasks[j]++;

        processes[i] = proc;
    }
    
    float total_f = cpu_total(cpu)/100;
    wipe_terminal();
    printf("Tasks: %d total, %2d running, %2d sleeping, %2d stopped, %2d zombie\n", res, tasks[0], tasks[1]+tasks[2], tasks[3], tasks[4]);

    printf("%%Cpu(s):  %.1f us,  %.1f sy,  %.1f ni, %.1f id, %.1f wa,  %.1f hi,  %.1f si,  %.1f st\n",
                            (float)cpu->user_time/total_f,
                            (float)cpu->sys_time/total_f, 
                            (float)cpu->userlow_time/total_f,
                            (float)cpu->idle_time/total_f,
                            (float)cpu->iowait_time/total_f,
                            (float)cpu->irq_time/total_f,
                            (float)cpu->softirq_time/total_f,
                            (float)cpu->steal_time/total_f);

    printf("KiB Mem :  %lu total,  %lu free,   %lu used,   %lu buff/cache\n"
       "KiB Swap:  %lu total,  %lu free,   %lu used.   %lu avail Mem\n",
       mem->total, mem->free, used_mem, 
       mem->buffers+mem->cached, mem->swap_total, mem->swap_free, mem->swap_total-mem->swap_free-mem->swap_cached,
       mem->avail);

    qsort(processes, res, sizeof(Process*), cmp_on_cpu_usage);
    
    printf("%7s %7s %4s %4s %7s %7s %7s %2s %7s %7s %9s %8s\n", "PID", "USER", "PR", "NI", "VIRT", "RES", "SHR", "S", "%CPU", "%MEM", "TIME+", "COMMAND");
    for(int i = 0; i<res && i < limit; i++){
        long unsigned time = processes[i]->utime+processes[i]->stime;
        printf("%7d %7d %4ld %4ld %7lu %7u %7u %2c %7.1f %7.1f %5lu:%lu,%lu %8s\n",
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
                                                time/3600,
                                                (time/60)%60,
                                                time%60,
                                                processes[i]->command
                                                );
    }
    //free heap memory
    for(int i = 0; i<res; i++) free(processes[i]);
    free(processes);
    free(cpu);
    free(mem);
    return 1;
}

void ctrlc_handler(int sig){
    exit_cond = 1;
}

inline void wipe_terminal(){ write(STDOUT_FILENO, "\x1b\x5b\x48\x1b\x5b\x32\x4a\x1b\x5b\x33\x4a", 11); }