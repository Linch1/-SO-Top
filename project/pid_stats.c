#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include "./pid_info.h";
#include "./pid_stats.h";
#include<time.h>
#include <unistd.h>
#define SC_CLK_TCK 2

float getSystemUptimeSec(){
    // Try to open /proc/uptime.
    char path = "/proc/uptime"; 
    float uptime_sec, b;
    FILE *fp = fopen("/proc/uptime", "r");
    fscanf(fp, "%f %f", &uptime_sec, &b);
    return uptime_sec;
}
float getTotalCpuUsage( float uptime_sec, pstat stat ){
    float total_time = stat.utime_ticks + stat.stime_ticks; // time_spent_in_user_mode + time_spent_in_kernel_mode from process
    total_time+= stat.cutime_ticks + stat.cstime_ticks; // time spent in cpu from children processes
    long CLOCKS = sysconf(SC_CLK_TCK);
    float process_run_for_seconds = uptime_sec - (stat.start_time_ticks/CLOCKS);
    float cpu_usage = 100 * ( (total_time / CLOCKS) / process_run_for_seconds );
    return cpu_usage;
}
void getMemory(struct Memory* mem){

    FILE *f=fopen("/proc/meminfo", "r");

    char buff[256];
    while(fgets(buff, sizeof(buff), f)){
        if(sscanf(buff, "MemTotal: %d kB", &mem->Total) == 1) continue;
        
        if(sscanf(buff, "MemFree: %d kB", &mem->Free) == 1) continue;

        if(sscanf(buff, "MemAvailable: %d kB", &mem->Avail) == 1)continue;       
        
        if(sscanf(buff, "Cached: %d kB", &mem->Cache) == 1) break;
    }
    mem->Used = mem->Total - mem->Avail;
    
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

void calc_cpu_usage_pct(const struct pstat* cur_usage,
                        const struct pstat* last_usage,
                        double* usage)
{
    const long unsigned int pid_diff =
        ( cur_usage->utime_ticks + cur_usage->stime_ticks ) -
        ( last_usage->utime_ticks + last_usage->stime_ticks );

    *usage = 1/(float)SLEEP_INTERVAL * pid_diff;
}
void printPidStats( struct pstat stats ){
    printf(
        "utime_ticks: %d | cutime_ticks: %d | stime_ticks: %d | cstime_ticks: %d | vsize: %d | rss: %d | cpu_total_time: %d \n",
        stats.utime_ticks, stats.cutime_ticks, stats.stime_ticks, stats.cstime_ticks, stats.vsize, stats.rss, stats.cpu_total_time
    );
}

/*
 * read /proc data into the passed struct pstat
 * returns 0 on success, -1 on error
*/
int getPidStats(const pid_t pid, struct pstat* result) {
    //convert  pid to string
    char pid_s[20];
    snprintf(pid_s, sizeof(pid_s), "%d", pid);
    char stat_filepath[30] = "/proc/"; strncat(stat_filepath, pid_s,
            sizeof(stat_filepath) - strlen(stat_filepath) -1);
    strncat(stat_filepath, "/stat", sizeof(stat_filepath) -
            strlen(stat_filepath) -1);

    FILE *fpstat = fopen(stat_filepath, "r");
    if (fpstat == NULL) {
        perror("FOPEN ERROR ");
        return -1;
    }

    FILE *fstat = fopen("/proc/stat", "r");
    if (fstat == NULL) {
        perror("FOPEN ERROR ");
        fclose(fstat);
        return -1;
    }

    //read values from /proc/pid/stat
    bzero(result, sizeof(struct pstat));
    long int rss;
    if (fscanf(fpstat, 
                "%*d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u "
                "%lu %lu %ld %ld %d %d %*d %*d %lu %lu %ld",
                &result->procName,
                &result->utime_ticks, &result->stime_ticks,
                &result->cutime_ticks, &result->cstime_ticks, &result->priority, &result->nice, &result->start_time_ticks, &result->vsize,
                &rss) == EOF) {
        fclose(fpstat);
        return -1;
    }
    fclose(fpstat);

    
    result->rss = rss * getpagesize();

    //read+calc cpu total time from /proc/stat
    long unsigned int cpu_time[10];
    bzero(cpu_time, sizeof(cpu_time));
    if (fscanf(fstat, "%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                &cpu_time[0], &cpu_time[1], &cpu_time[2], &cpu_time[3],
                &cpu_time[4], &cpu_time[5], &cpu_time[6], &cpu_time[7],
                &cpu_time[8], &cpu_time[9]) == EOF) {
        fclose(fstat);
        return -1;
    }

    fclose(fstat);

    for(int i=0; i < 10;i++)
        result->cpu_total_time += cpu_time[i];

    return 0;
}

