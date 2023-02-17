#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include "./linked_list/linked_list.h";
#include "./pid_stats.h";
#include "./pid_info.h";
#include "./pid_cmd.h";
#include<time.h>
#include <unistd.h>

char i=0;

void pid_cmd_prompt(){
    char fun;
    int pid;
    scanf("%c %d",&fun, &pid);
    switch (fun)
    {
        case 'h': h();
            break;
        case 't': t(pid);
            break;
        case 'k': k(pid);
            break;
        case 'r': r(pid);
            break;
        case 's': su(pid);
            break;
        case 'q':
            exit(0);
            break;
    }
}

void system_info(){
    Memory mem;
    getMemory(&mem); 
    Swap swap;
    getSwap(&swap);

    printf("MemTotal: %d\nMemFree: %d\nMemAvailable: %d\nCached: %d\nMemUsed: %d\nSwapTotal: %d\nSwapFree: %d\nSwapUsed: %d\n",mem.Total,mem.Free,mem.Avail,mem.Cache,mem.Used,swap.Total,swap.Free,swap.Used);
    printf("\n");
    printf("Inserisci nel terminale la funzione da eseguire seguita dal process Id:\n");
    printf("\n");
    printf("Premendo h potrai vedere le funzioni eseguibili\n");
    printf("Premendo q potrai terminare il programma\n");
    //printf("Digita: ");
}

int main(void){

    float uptime_sec = getSystemUptimeSec(); // secondi di uptime del pc

    ListHead headStats; 
    List_init(&headStats); // inzializza lista stati (prec|corrente) dei vari processi
    

    while( i==0 ) {

        system("clear");
        ListHead head;
        List_init(&head); // inzializza lista pid processi running

        getRunningPids(&head); // popola la lista con tutti i processi running
        ListItem* aux= head.first;

        
        system_info(); // print system info

        printf("  PID       |      CPU      |      MEM      |    PRIO    |   NICE    |      NAME     \n");
        printf("-------------------------------------------------------------------------------------\n");
        while(aux && i==0){
            PidListItem* element = (PidListItem*) aux;

            PidStatListItem* s = PidListStat_find( &headStats, element->pid);

            if( s == NULL ){ // collect process stats for first time and add to linked list;
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
           
            printf("%5d             ", element->pid);
            printf("%.02f           ", usage);
            printf("   --           ");
            printf("%3i         ", s->stat.current.priority);
            printf("%3i             ", s->stat.current.nice); 
            printf("%-20s       ", s->stat.current.procName );
          
            printf("\n");

            aux=aux->next;
        }
        List_free( &head );
        sleep( SLEEP_INTERVAL );
    }

    
}