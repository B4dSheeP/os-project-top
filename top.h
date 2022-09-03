#pragma once


#define error(s) if(1==1){ fprintf(stderr, "%s\n", s); exit(-1);};
#define STATUSES "RSITZ"

typedef long long unsigned llu_int;

typedef struct {
    llu_int user_time;
    llu_int userlow_time;
    llu_int sys_time;
    llu_int idle_time;
    llu_int iowait_time;
    llu_int irq_time;
    llu_int softirq_time;
    llu_int steal_time;
    llu_int guest_time;
    llu_int guestnice_time;
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
    llu_int starttime;
    unsigned res;
    unsigned shr;
    char s;
    float cpu_percentage; 
    float mem_percentage;
    llu_int cpu_time;
    char command[200]; 

    long unsigned utime;
    long unsigned stime;
} Process;


bool print_top(unsigned);
void wipe_terminal();
void ctrlc_handler(int);