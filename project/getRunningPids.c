#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include "./linked_list/linked_list.h";
#include "./getPidStats.h";
#include<time.h>
#include <unistd.h>

#define SC_CLK_TCK 2
typedef struct PidStat {
    pstat current;
    pstat prev;
} PidStat;


typedef struct PidListItem{
  ListItem list;
  pid_t pid;
} PidListItem;

typedef struct PidStatListItem{
  ListItem list;
  pid_t pid;
  PidStat stat;
} PidStatListItem;


void PidList_print(ListHead* head){
  ListItem* aux=head->first;
  printf("[");
  while(aux){
    PidListItem* element = (PidListItem*) aux;
    printf("%d ", element->pid);
    aux=aux->next;
  }
  printf("]\n");
}
PidStatListItem* PidListStat_find(ListHead* head, pid_t pid){
  ListItem* aux=head->first;
  while(aux){
    PidStatListItem* element = (PidStatListItem*) aux;
    if( element->pid == pid ) return element;
    aux = aux->next;
  }
  return NULL;
}


// Helper function to check if a struct dirent from /proc is a PID directory.
int is_pid_dir(const struct dirent *entry) {
    const char *p;

    for (p = entry->d_name; *p; p++) {
        if (!isdigit(*p))
            return 0;
    }

    return 1;
}

int getRunningPids( ListHead* head ) {
    DIR *procdir;
    FILE *fp;

    

    struct dirent *entry;
    char path[256 + 5 + 5]; // d_name + /proc + /stat
    int pid;
    unsigned long maj_faults;

    // Open /proc directory.
    procdir = opendir("/proc");
    if (!procdir) {
        perror("opendir failed");
        return 1;
    }

    // Iterate through all files and directories of /proc.
    while ((entry = readdir(procdir))) {
        // Skip anything that is not a PID directory.
        if (!is_pid_dir(entry))
            continue;

        // Try to open /proc/<PID>/stat.
        snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
        fp = fopen(path, "r");

        if (!fp) {
            perror(path);
            continue;
        }

        // Get PID, process name and number of faults.
        fscanf(fp, "%d %s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %lu",
            &pid, &path, &maj_faults
        );

        PidListItem* new_element= (PidListItem*) calloc(1, sizeof(PidListItem));
        if (! new_element) {
            printf("out of memory\n");
            break;
        }
        new_element->pid = pid;
        
        ListItem* result= List_insert(head, head -> last, (ListItem*) new_element);
        assert(result);

        // Pretty print.
        // printf("%5d %-20s: %lu\n", pid, path, maj_faults);
        fclose(fp);
    }

    

    closedir(procdir);
    return 0;
}

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

PidStatListItem* intializeProcessStats( ListHead *head, pid_t pid ){
    pstat stat;
    getPidStats( pid, &stat );


    PidStat* new_stat= (PidStat*)
    malloc(sizeof(PidStat));
    new_stat->current = stat;


    PidStatListItem* new_element = (PidStatListItem*) calloc(1, sizeof(PidStatListItem));
    new_element->pid = pid;
    new_element->stat = *new_stat;

    ListItem* result= List_insert( head, head->last, (ListItem*) new_element);
    assert(result);

    return new_element;
}

int main(void){

    float uptime_sec = getSystemUptimeSec(); // secondi di uptime del pc

    ListHead headStats; 
    List_init(&headStats); // inzializza lista stati (prec|corrente) dei vari processi
    

    while( 1 ) {

        fflush(NULL); // clear console

        ListHead head;
        List_init(&head); // inzializza lista pid processi running

        getRunningPids(&head); // popola la lista con tutti i processi running
        ListItem* aux= head.first;

        while(aux){
            PidListItem* element = (PidListItem*) aux;

            PidStatListItem* s = PidListStat_find( &headStats, element->pid);

            if( s == NULL ){ // collect process stats for first time andd add to linked list;
                s = intializeProcessStats( &headStats, element->pid );
                
                continue;
            }
            else {
                pstat stat;
                getPidStats( element->pid, &stat );
                s->stat.prev = s->stat.current;
                s->stat.current = stat;
            }
            
            double usage;
            calc_cpu_usage_pct(&s->stat.current, &s->stat.prev, &usage);
            printf("%d %%cpu: %.02f\n", element->pid, usage);

            

            aux=aux->next;
        }
        List_free( &head );
        
        sleep( SLEEP_INTERVAL );
    }

    
}