#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include "./linked_list/linked_list.h";
#include "./getPidStats.h";
#include "./getRunningPids.h";
#include "function.h"
#include<time.h>
#include <unistd.h>

int main(void){

    float uptime_sec = getSystemUptimeSec(); // secondi di uptime del pc

    ListHead headStats; 
    List_init(&headStats); // inzializza lista stati (prec|corrente) dei vari processi
    char i=0;

    while( i==0 ) {

        fflush(NULL); // clear console

        ListHead head;
        List_init(&head); // inzializza lista pid processi running

        getRunningPids(&head); // popola la lista con tutti i processi running
        ListItem* aux= head.first;

        Memory mem;
        getMemory(&mem); 

        Swap swap;
        getSwap(&swap);
        
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
            printf("%d %%cpu: %.02f\n", element->pid, usage);

            printf("MemTotal: %d\nMemFree: %d\nMemAvailable: %d\nCached: %d\nMemUsed: %d\nSwapTotal: %d\nSwapFree: %d\nSwapUsed: %d\n",mem.Total,mem.Free,mem.Avail,mem.Cache,mem.Used,swap.Total,swap.Free,swap.Used);
            printf("\n");
            printf("Inserisci nel terminale la funzione da eseguire seguita dal process Id:\n");
            printf("\n");
            printf("Premendo h potrai vedere le funzioni eseguibili\n");
            printf("Premendo q potrai terminare il programma\n");
            printf("Digita: ");
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
                    i=1;
                    break;
            }
            getMemory(&mem);
            getSwap(&swap);
            aux=aux->next;
        }
        List_free( &head );
        
        sleep( SLEEP_INTERVAL );
    }

    
}