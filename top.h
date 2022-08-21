#pragma once


#define error(s) if(1==1){ fprintf(stderr, "%s\n", s); exit(-1);};
#define STATUSES "RSITZ"

typedef struct {
    long long unsigned user_time;
    long long unsigned userlow_time;
    long long unsigned sys_time;
    long long unsigned idle_time;
    long long unsigned iowait_time;
    long long unsigned irq_time;
    long long unsigned softirq_time;
    long long unsigned steal_time;
    long long unsigned guest_time;
    long long unsigned guestnice_time;
} Cpu;

typedef struct {
    unsigned long total;
    unsigned long free;
    unsigned long avail; //used è total-free-buffers-cached
    unsigned long buffers;
    unsigned long cached;
    unsigned long swap_total;
    unsigned long swap_free; //swap used è total-free-swapcached
    unsigned long swap_cached;
    

} Mem;

typedef struct {
    pid_t pid;
    int user;
    long int pr;
    long int ni;
    long unsigned virt;
    long long unsigned starttime;
    unsigned res;
    unsigned shr;
    char s;
    float cpu_percentage; 
    float mem_percentage;
    long long unsigned cpu_time;
    char command[200]; 

    long unsigned utime;
    long unsigned stime;
} Process;



void print_top(unsigned, unsigned);
void wipe_terminal();