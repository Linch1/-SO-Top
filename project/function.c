#include "function.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <errno.h>

//Process Function
static void check_error(long res, char* msg, int proc) {
    if (res != -1) printf("Process PID %d successed %s\n",proc, msg);
    switch (errno){
    case EPERM: printf("you haven't permission to %s process PID %d\n", msg ,proc);
    case ESRCH: printf("Process PID %d  not exist \n",proc);
    }
}

void h(){
    printf("s: suspend\n");
    printf("r: resume\n");
    printf("t: terminate\n");
    printf("k: kill\n");
    printf("u: reload\n");
    printf("q: quit\n");
}

void t(int pid){
    int res=kill(pid,SIGTERM);
    check_error(res,'Terminate',pid);
}

void k(int pid){
    int res=kill(pid,SIGKILL);
    check_error(res,'Kill',pid);
}

void r(int pid){
    int res=kill(pid,SIGCONT);
    check_error(res,'resume',pid);
}

void s(int pid){
    int res=kill(pid,SIGSTOP);
    check_error(res,'suspend',pid);
}

//Memory Function

void getMemory(struct Memory* mem){

    FILE *f=fopen("/proc/meminfo", "r");

    char buff[256];
    while(fgets(buff, sizeof(buff), f)){
        if(sscanf(buff, "MemTotal: %d kB", &mem->Total) == 1) continue;
        
        if(sscanf(buff, "MemFree: %d kB", &mem->Free) == 1) continue;

        if(sscanf(buff, "MemAvailable: %d kB", &mem->Avail) == 1)continue;       
        
        if(sscanf(buff, "Cached: %d kB", &mem->Cache) == 1) break;
    }
    mem->Used = mem.Total - mem.Avail;
    
    if(fclose(f) != 0) exit(-1);
}

void getSwap(struct Swap* swap){

    FILE *f=fopen("/proc/meminfo", "r");

    char buff[256];
    while(fgets(buff, sizeof(buff), f)){
        if(sscanf(buff, "SwapTotal: %d kB", &swap->Total) == 1) continue;
        
        if(sscanf(buff, "SwapFree: %d kB", &swap->Free) == 1) break;
    }
    swap->Used = swap->Total - swap->Free;

    if(fclose(f) != 0) exit(-1);
}
