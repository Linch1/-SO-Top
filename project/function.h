// Memory Struct and Function
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

void getMemory(struct Memory* mem);
void getSwap(struct Swap* swap);

// Process Function 
void h();
void t(int pid);
void k(int pid);
void r(int pid);
void su(int pid);

