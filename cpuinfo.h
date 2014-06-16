#ifndef CPU_INFO_H
#define CPU_INFO_H
#include "top_config.h"
typedef unsigned long cputime_t;
typedef struct cpuinfo
{
	char cpuname[8];
	cputime_t user;         //从系统启动开始累计到当前时刻，处于用户态的运行时间，不包含 nice值为负进程。
	cputime_t nice;		//从系统启动开始累计到当前时刻，nice值为负的进程所占用的CPU时间
	cputime_t system;	//从系统启动开始累计到当前时刻，处于核心态的运行时间
	cputime_t idle;		//从系统启动开始累计到当前时刻，除IO等待时间以外的其它等待时间
	cputime_t iowait;	//从系统启动开始累计到当前时刻，IO等待时间(since 2.5.41)
	cputime_t irq;		//从系统启动开始累计到当前时刻，硬中断时间(since 2.6.0-test4)
	cputime_t softirq;	//从系统启动开始累计到当前时刻，软中断时间(since 2.6.0-test4)
	cputime_t stealstolen;	//which is the time spent in other operating systems when running in a 
                                //virtualized environment(since 2.6.11)
	cputime_t guest;	//which is the time spent running a virtual CPU for guest operating systems 
}cpuinfo_s;
typedef struct cpu_time
{ 
    cpuinfo_s * new_cpu_info;
    cpuinfo_s * old_cpu_info;
    cputime_t * new_total;
    cputime_t * old_total;
}cpu_time_s;
void cpuinfo_view(cpu_time_s * cpu);
void get_cpuinfo(cpu_time_s * cpu);
int init_cpuinfo(cpu_time_s * cpu);
void update_cpuinfo(cpu_time_s * cpu);
#endif
