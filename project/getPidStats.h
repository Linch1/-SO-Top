#pragma once
#include <sys/types.h>
#define SLEEP_INTERVAL 1

typedef struct pstat {
    long unsigned int utime_ticks; // tempo speso in ambiente utente
    long int cutime_ticks; // tempo speso in ambiente kernel

    
    long unsigned int stime_ticks; // tempo speso in ambiente utente dai child processo
    long int cstime_ticks;// tempo speso in ambiente kernel dai child processo
    long unsigned int vsize; // virtual memory size in bytes
    long unsigned int rss; // Resident  Set  Size in bytes
    long unsigned int cpu_total_time; //
    long unsigned int start_time_ticks; // orario di avvio processo
} pstat;

int getPidStats(const pid_t pid, struct pstat* result);
