#include <ncurses.h>
#include "pid_cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <errno.h>


//Process Function
static void check_error(WINDOW* win,long res, char* msg, int proc) {
    if (res != -1) {
        mvwprintw(win,5,1,"Il Processo PID( %d ) %s con successo",proc, msg);
    }else{
        switch (errno){
                case EPERM: {wmove(win, 5, 1);
                             wclrtoeol(win);
                             mvwprintw(win,5,1,"Non hai il permesso di %s il processo PID( %d )", msg ,proc); 
                             break;}
                case ESRCH: {wmove(win, 5, 1);
                             wclrtoeol(win);
                             mvwprintw(win,5,1,"Il Processo PID( %d ) non esiste ",proc);
                             break;}
        }
    }
}

void h(WINDOW* win){
    wrefresh(win);
    wmove(win, 5, 1);
    wclrtoeol(win);
    mvwprintw(win,5,1,"s: suspend  ||  r: resume || t: terminate || k: kill || q: quit");
    wmove(win, 6, 1);
    wclrtoeol(win);
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


