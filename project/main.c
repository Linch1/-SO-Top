#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include "./linked_list/linked_list.h";
#include "./getPidStats.h";
#include "./getRunningPids.h";
#include "./function.h"
#include<time.h>
#include <unistd.h>

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

        Memory mem;
        getMemory(&mem); 

        Swap swap;
        getSwap(&swap);

        while(aux){
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
                        

            aux=aux->next;
        }
        printf("MemTotal: %d\nMemFree: %d\nMemAvailable: %d\nCached: %d\nMemUsed: %d\nSwapTotal: %d\nSwapFree: %d\nSwapUsed: %d\n",mem.Total,mem.Free,mem.Avail,mem.Cache,mem.Used,swap.Total,swap.Free,swap.Used);
        //primo abbozzo di interazione con i processi
        while(1){
            printf("Inserisci nel terminale la funzione da eseguire seguita dal process Id:\n");
            printf("\n");
            printf("Premendo h potrai vedere le funzioni eseguibili\n");
            printf("Premendo q potrai terminare il programma\n");
            printf("Digita: ")
            char *fun;
            int *pid;
            scanf("%s %d",fun, pid);
            switch (fun)
            {
            case h: h();
            case t: t(pid);
            case k: k(pid);
            case r: r(pid);
            case s: s(pid);
            case q: break;
            default:  printf("carattere non riconosciuto\n");
                      continue;
                     
            }
            free(fun);
        }
        free(mem);
        free(swap);
        List_free( &head );
        
        sleep( SLEEP_INTERVAL );
    }

    
}