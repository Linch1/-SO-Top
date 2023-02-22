#include <ncurses.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <pthread.h>
#include "linked_list.h"
#include "pid_stats.h"
#include "pid_info.h"
#include "pid_cmd.h"
#include <sys/ioctl.h>



#define PROC_WIN_HEIGHT 20
#define PROC_WIN_WIDTH 165
#define CMD_WIN_HEIGHT 8
#define CMD_WIN_WIDTH 165
#define SLEEP_INTERVAL 5


void draw_proc_window(WINDOW* win2){
    float uptime_sec = getSystemUptimeSec(); // secondi di uptime del pc

    ListHead headStats; 
    List_init(&headStats); // inzializza lista stati (prec|corrente) dei vari processi
    int cnt=5,i=0;
    Memory mem;
    Swap swap;
    
    while(i==0){
        ListHead head;
        List_init(&head); // inzializza lista pid processi running
        getRunningPids(&head); // popola la lista con tutti i processi running
        ListItem* aux= head.first;
        scrollok(win2,TRUE);
        while(aux) {
            if(cnt>18) cnt=5;
            PidListItem* element = (PidListItem*) aux;

            PidStatListItem* s = PidListStat_find(&headStats, element->pid);

            if(s == NULL) {
                s = intializeProcessStats(&headStats, element->pid);
                continue;
            } else {
                pstat stat;
                getPidStats(element->pid, &stat);
                s->stat.prev = s->stat.current;
                s->stat.current = stat;
            }

            double usage;
            getMemory(&mem); //popola la struct mem

            getSwap(&swap);//popola la struct swap

            calc_cpu_usage_pct(&s->stat.current, &s->stat.prev, &usage);

            init_pair(1, COLOR_GREEN, COLOR_BLACK); //COLORI PER MESSAGGIO STATICO
            
            wattron(win2,COLOR_PAIR(1));

            mvwprintw(win2,1,1,"MemTotal: %d || MemFree: %d || MemAvailable: %d || Cached: %d || MemUsed: %d  || SwapTotal: %d || SwapFree: %d || SwapUsed: %d ",mem.Total,mem.Free,mem.Avail,mem.Cache,mem.Used,swap.Total,swap.Free,swap.Used);

            mvwprintw(win2,3,25, " PID       |      CPU      |      MEM      |    PRIO    |   NICE    |      NAME     ");

            wattroff(win2,COLOR_PAIR(1));

            mvwprintw(win2,cnt,25, "%5d            %.02f              --            %3i         %3i           %-20s       ", element->pid, usage, s->stat.current.priority, s->stat.current.nice, s->stat.current.procName);

            aux = aux->next;

            cnt++;

        }
        sleep( SLEEP_INTERVAL );
        scrollok(win2,FALSE);
        wrefresh(win2);

        List_free( &head );
    }

}
void draw_cmd_window(WINDOW* win3) {
    
    init_pair(1,COLOR_GREEN, COLOR_BLACK);  //COLORI PER MESSAGGIO STATICO

    char fun;
    int pid;
    
    while(1){
    wattron(win3,COLOR_PAIR(1));
    mvwprintw(win3,1,1," Inserisci nel terminale la funzione da eseguire seguita dal process Id:");
    mvwprintw(win3,2,1," Premendo h potrai vedere le funzioni eseguibili");
    mvwprintw(win3,3,1," Premendo q potrai uscire dal programma");
    wattroff(win3,COLOR_PAIR(1));

    echo();
    wmove(win3, 4, 1);
    wclrtoeol(win3);
    wmove(win3,4,1);
    wscanw(win3, "%c %d", &fun, &pid);
    switch (fun)
    {
        case 'h': h(win3);
            break;
        case 't': t(win3,pid);
            break;
        case 'k': k(win3,pid);
            break;
        case 'r': r(win3,pid);
            break;
        case 's': su(win3,pid);
            break;
        case 'q':
            system("clear");
            exit(0);
            break;
    }
    }
}


int main() {
    initscr();
    noecho();
    cbreak();
    start_color();

    // create info window
    WINDOW* proc = newwin(PROC_WIN_HEIGHT, PROC_WIN_WIDTH,0, 0);
    WINDOW* cmd = newwin(CMD_WIN_HEIGHT,CMD_WIN_WIDTH,PROC_WIN_HEIGHT ,0);

    box(proc,0,0);
    box(cmd,0,0);

    
    wrefresh(proc);
    wrefresh(cmd);

    // create threads for info window, process window, and command windowq
    pthread_t proc_thread, cmd_thread;
    
    pthread_create(&proc_thread, NULL,(void*) draw_proc_window,(void*) proc);
    pthread_create(&cmd_thread, NULL,(void*) draw_cmd_window,(void*) cmd);

    // wait for threads to finish
    pthread_join(cmd_thread, NULL);

    endwin();
    return 0;
}