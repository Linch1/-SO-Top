#include "pid_cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <errno.h>

//Process Function
static void check_error(long res, char* msg, int proc) {
    if (res != -1) printf("Process PID %d successed %s\n",proc, msg);
    switch (errno){
    case EPERM: printf("you havent permission to %s process PID %d\n", msg ,proc);
    case ESRCH: printf("Process PID %d  not exist \n",proc);
    }
}

void h(){
    printf("\n");
    printf("s: suspend\n");
    printf("r: resume\n");
    printf("t: terminate\n");
    printf("k: kill\n");
    printf("u: reload\n");
    printf("q: quit\n");
    printf("\n");
}

void t(int pid){
    check_error(kill(pid,SIGTERM),"terminate",pid);
}

void k(int pid){
    check_error(kill(pid,SIGKILL),"kill",pid);
}

void r(int pid){
    check_error(kill(pid,SIGCONT),"resume",pid);
}

void su(int pid){
    check_error(kill(pid,SIGSTOP),"suspend",pid);
}

