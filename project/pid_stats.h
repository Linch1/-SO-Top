typedef struct Memory{
    int Total;
    int Free;
    int Avail;
    int Used;
    int Cache;
}Memory;

typedef struct Swap{
    int Total;
    int Free;
    int Used;
}Swap;

float getSystemUptimeSec();
float getTotalCpuUsage( float uptime_sec, pstat stat );
void getMemory(struct Memory* mem);
void getSwap(struct Swap* swap);
void printPidStats( struct pstat stats );
void calc_cpu_usage_pct(const struct pstat* cur_usage, const struct pstat* last_usage, double* usage);
int getPidStats(const pid_t pid, struct pstat* result);