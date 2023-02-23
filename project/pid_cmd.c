#include <ncurses.h>
#include "pid_cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <errno.h>


//Process Function
static void check_error(WINDOW* win,long res, char* msg, int proc) {
    //COLORI PER LE RISPOSTE
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

    if (res != -1) {
        wattron(win,COLOR_PAIR(2));
        mvwprintw(win,5,1," Il Processo PID( %d ) %s con successo",proc, msg);
        wattroff(win,COLOR_PAIR(2));
    }else{
        switch (errno){
                case EPERM: {wmove(win, 5, 1);
                             wclrtoeol(win);
                             wattron(win,COLOR_PAIR(1));
                             mvwprintw(win,5,1,"Non hai il permesso di %s il processo PID( %d ) ", msg ,proc);
                             wattroff(win,COLOR_PAIR(1)); 
                             break;}
                case ESRCH: {wmove(win, 5, 1);
                             wclrtoeol(win);
                             wattron(win,COLOR_PAIR(1));
                             mvwprintw(win,5,1,"Il Processo PID( %d ) non esiste ",proc);
                             wattroff(win,COLOR_PAIR(1));
                             break;}
        }
    }
}



void t(WINDOW* win,int pid){
    check_error(win,kill(pid,SIGTERM),"terminate",pid);
}

void k(WINDOW* win,int pid){
    check_error(win,kill(pid,SIGKILL),"kill",pid);
}

void r(WINDOW* win,int pid){
    check_error(win,kill(pid,SIGCONT),"resume",pid);
}

void su(WINDOW* win,int pid){
    check_error(win,kill(pid,SIGSTOP),"suspend",pid);
}


