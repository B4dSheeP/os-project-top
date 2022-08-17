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
    int pr;
    int ni;
    unsigned virt;
    float cpu_percentage; 
    float mem_percentage;
    char command[200]; 
} Process;



void print_top();
