#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include "./linked_list/linked_list.h";
#include "./getPidStats.h";
#include "./getRunningPids.h";
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