// Memory Function
struct Memory{
    int Total;
    int Free;
    int Avail;
    int Used;
    int Cache;
};

struct Swap{
    int Total;
    int Free;
    int Used;
};

struct Memory getMemory();
struct Swap getSwap();

// Process Function 
void h();
void t(int pid);
void k(int pid);
void r(int pid);
void s(int pid);

