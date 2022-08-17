#pragma once



typedef struct {


} Cpu;


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
    char user[10];
    long int pr;
    long int ni;
    long unsigned virt;
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



void print_top();
void wipe_terminal();