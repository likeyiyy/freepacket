#ifndef TASK_INFO_H
#define TASK_INFO_H
//typedef int pid_t;
#include <unistd.h>
#include <sys/time.h>
#include "hash.h"
#include "meminfo.h"
#include "cpuinfo.h"
typedef struct taskinfo 
{
  int           pid;                      /** The process id. **/
  char          name[128]; 				  /** The filename of the executable **/
  char          state; /** 1 **/          /** R is running, S is sleeping, **/
  //task time 
  unsigned long utime;                    /** user mode jiffies                                         **/
  unsigned long stime;                    /** kernel mode jiffies                                       **/
  unsigned long	cutime;                   /** user mode jiffies with childs                             **/
  unsigned long cstime;                   /** kernel mode jiffies with childs                           **/
  unsigned long total;
  unsigned long start_time;
  
  double use_cpu;
  double sys_cpu;
  double task_cpu;
  //mem info
  double use_mem;
  unsigned int  vsize;                    /** Virtual memory size                                       **/
  unsigned int  rss;                      /** Resident Set Size                                         **/ 
  unsigned int  shares;                   /** share memory size                                         **/
  struct timeval timestamp;
  struct taskinfo * next;
}taskinfo;
typedef struct machine
{
    cpu_time_s * CPU;
    meminfo_s * MEMINFO;  
}machine_s;
void read_one_task_info(pid_t pid,taskinfo * tinfo,machine_s * MACHINE);
void get_task_info(void);
void view_task_info(void);
const char * format_task_info(taskinfo * task);
struct taskinfo * new_task();
void free_task(struct taskinfo *task);
pid_t get_process_by_name(const char * name);
pid_t get_process_by_id(pid_t pid);

#endif
