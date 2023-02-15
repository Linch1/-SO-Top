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


void PidList_print(ListHead* head);
PidStatListItem* PidListStat_find(ListHead* head, pid_t pid);

// Helper function to check if a struct dirent from /proc is a PID directory.
int is_pid_dir(const struct dirent *entry);
int getRunningPids( ListHead* head );
float getSystemUptimeSec();
float getTotalCpuUsage( float uptime_sec, pstat stat );
PidStatListItem* intializeProcessStats( ListHead *head, pid_t pid );

