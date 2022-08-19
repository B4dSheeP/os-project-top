#pragma once


#define error(s) if(1==1){ fprintf(stderr, "%s\n", s); exit(-1);};


typedef struct {
    long long unsigned user_time;
    long long unsigned userlow_time;
    long long unsigned sys_time;
    long long unsigned idle_time;
    long long unsigned iowait_time;
    long long unsigned irq_time,
    long long unsigned softirq_time,
    long long unsigned steal_time,
    long long unsigned guest_time,
    long long unsigned guestnice_time

} Cpu;

inline long long unsigned cpu_total(Cpu* cpu){
    long long unsigned* cpu_as_array = (long long unsigned*)cpu;
    long long unsigned r = 0;
    for(int i=0; i<10; i++) r += cpu_as_array[i];
    return r;
}


typedef struct {
    unsigned total;
    unsigned free;
    unsigned used;
    unsigned cache;
    unsigned swap_total;
    unsigned swap_free;
    unsigned swap_used;
    unsigned avail;

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