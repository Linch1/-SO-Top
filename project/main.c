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

#define CMD_WIN_HEIGHT 10
#define CMD_WIN_WIDTH 170
#define PROC_WIN_HEAD_HEIGHT 4
#define PROC_WIN_HEAD_WIDTH 170
#define PROC_WIN_HEIGHT 20
#define PROC_WIN_WIDTH 170

#define SLEEP_INTERVAL 0.5


WINDOW* cmd;
WINDOW* proc_head;
WINDOW* proc;
pid_t CURRENT_PID = 1;
int current_row = 0;
int scroll_top = 0;
int scroll_bot = PROC_WIN_HEIGHT - 1;



void PidListfree(ListHead* head) {
  ListItem* aux=head->first;
  while(aux){
    ListItem* tmp = aux->next;
    free(((PidListItem*)aux)->name);
    free(aux);
    aux = tmp;
  }
}
void draw_proc_window(){
    float uptime_sec = getSystemUptimeSec(); // secondi di uptime del pc

    ListHead headStats; 
    List_init(&headStats); // inzializza lista stati (prec|corrente) dei vari processi
    int row=5,i=0;
    Memory mem;
    Swap swap;
    
    while(i==0){

        ListHead head;
        List_init(&head); // inzializza lista pid processi running
        getRunningPids(&head); // popola la lista con tutti i processi running
        ListItem* aux= head.first;
        //wclear(proc);
        

        init_pair(1, COLOR_GREEN, COLOR_BLACK); //COLORI PER MESSAGGIO STATICO
        wattron(proc_head,COLOR_PAIR(1));
        wclear(proc_head);
        wprintw(
            proc_head, 
            "MemTotal: %d || MemFree: %d || MemAvailable: %d || Cached: %d || MemUsed: %d  || SwapTotal: %d || SwapFree: %d || SwapUsed: %d \n", 
            mem.Total,mem.Free,mem.Avail,mem.Cache,mem.Used,swap.Total,swap.Free,swap.Used
        );
        wprintw(proc_head, "N   | PID       |      CPU        |         MEM         |     PRIO      |    NICE      |      NAME     \n");
        wattroff(proc_head,COLOR_PAIR(1));
        wrefresh(proc_head);

        werase(proc);
        row=0;
        while(aux) {
            
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

            if ( row >= scroll_top && row <= scroll_bot ){
                //wmove(proc, current_row - scroll_top + 1, 0);
                wprintw( 
                    proc, 
                    "%3d | %5d            %.02f               %6.02f             %3i           %3i           %-35s       \n", 
                    row, element->pid, usage, s->stat.current.ramUsage, s->stat.current.priority, s->stat.current.nice, element->name
                );
               
                wrefresh(proc);
            }

            aux = aux->next;
            row++;
        }
        
        
        sleep( SLEEP_INTERVAL );
        PidListfree( &head );
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

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    start_color();
    


    // create info window
    cmd = newwin(CMD_WIN_HEIGHT, CMD_WIN_WIDTH, 0 ,0);
    proc_head = newwin(PROC_WIN_HEAD_HEIGHT, PROC_WIN_HEAD_WIDTH, CMD_WIN_HEIGHT, 0);
    proc = newwin(PROC_WIN_HEIGHT, PROC_WIN_WIDTH, CMD_WIN_HEIGHT + PROC_WIN_HEAD_HEIGHT, 0);

    // Enable scrolling on proc window
    scrollok(proc, TRUE);
   
   

    box(proc_head,0,0);
    box(proc,0,0);
    box(cmd,0,0);
    
    wrefresh(proc_head);
    wrefresh(proc);
    wrefresh(cmd);
    
    // create threads for info window, process window, and command windowq
    pthread_t proc_thread, cmd_thread;
    
    pthread_create(&proc_thread, NULL,(void*) draw_proc_window, NULL);
    //pthread_create(&cmd_thread, NULL,(void*) draw_cmd_window,(void*) cmd);


    // Handle user input
    int ch;
    int scrollAmt = PROC_WIN_HEIGHT;
    while( (ch = getch()) != KEY_F(1)  ){
        switch (ch) {
            case KEY_UP:
                if (current_row > 0) {
                    current_row -= scrollAmt;
                }
                if (current_row < scroll_top) {
                    scroll_top -= scrollAmt;
                    scroll_bot -= scrollAmt;
                }
                break;
            case KEY_DOWN:
                current_row += scrollAmt;
                if (current_row > scroll_bot) {
                    scroll_top += scrollAmt;
                    scroll_bot += scrollAmt;
                }
                break;
            default:
                break;
        }
    }
    // Handle user input
    


    pthread_join(cmd_thread, NULL);
    endwin();
    return 0;
}