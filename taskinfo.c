#include "taskinfo.h"
#include "cpuinfo.h"
#include "meminfo.h"
#include <unistd.h>
#include <ctype.h>
#include "hash.h"
#include "top_config.h"
#include <string.h>
#define bytetok(x)	(((x) + 512) >> 10)
#define pagetok(x)	((x) << (PAGE_SHIFT - 10))
#if 0
/* forward definitions for comparison functions */
static int compare_pid(taskinfo ** pp1,taskinfo ** pp2)
{
	register taskinfo * p1;
	register taskinfo * p2;
	register long result;
	double dresult;
	p1 = *pp1;
	p2 = *pp2;
	return (p1->pid == p2->pid) ? 0: (p1->pid < p2->pid) ? -1 : 1;
}
int compare_cpu(taskinfo ** pp1,taskinfo ** pp2)
{
	register taskinfo * p1;
	register taskinfo * p2;
	p1 = *pp1;
	p2 = *pp2;
	if(p1->task_cpu > p2->task_cpu)
	{
		return -1;
	}
	else if(p1->task_cpu < p2->task_cpu)
		return 1;
	else
		return 0;
}
int compare_size(taskinfo ** pp1,taskinfo ** pp2)
{
	register taskinfo * p1;
	register taskinfo * p2;
	p1 = *pp1;
	p2 = *pp2;
	return (p1->vsize == p2->vsize)?0:(p1->vsize > p2->vsize)?-1:1;
}
int compare_res(taskinfo ** pp1,taskinfo ** pp2)
{
	register taskinfo * p1;
	register taskinfo * p2;
	p1 = *pp1;
	p2 = *pp2;
	return (p1->rss == p2->rss)?0:(p1->rss > p2->rss)?-1:1;
}

int (*task_compares[])(taskinfo ** pp1,taskinfo ** pp2) = {
    compare_pid,
	compare_cpu,
    compare_size,
    compare_res,
    NULL };
#endif
static inline char * skip_ws(const char *p)
{
        while (isspace(*p)) p++;
            return (char *)p;
}
    
static inline char * skip_token(const char *p)
{
    while (isspace(*p)) p++;
    while (*p && !isspace(*p)) p++;
    return (char *)p;
}

/*
 * Process structures are allocated and freed as needed.  Here we
 * keep big pools of them, adding more pool as needed.  When a
 * top_task structure is freed, it is added to a freelist and reused.
 */

static struct taskinfo *freelist = NULL;
static struct taskinfo *taskblock = NULL;
static struct taskinfo *taskmax = NULL;
/* 
 * 注意这里并没有freelist的初始化，而事实上，运行时会初始化。
 * 首先分配taskblock，分配PROCBLOCK_SIZE个taskblock，并且返回一个。
 * 假如下次分配之前，调用过free_task，那么freelist就有项了，就会使用freelist里的。
 * 假如下次分配之前，没有调用过free_task，那么将继续使用taskblock里面的项，
 * 当taskblock使用完之后，又会继续分配。
 */ 
struct taskinfo * new_task()
{
    struct taskinfo *p;

    if (freelist)
    {
		p = freelist;
		freelist = freelist->next;
    }
    else if (taskblock)
    {
		p = taskblock;
		if (++taskblock >= taskmax)
		{
	    	taskblock = NULL;
		}
    }
    else
    {
		p = taskblock = (struct taskinfo *)calloc(PROCBLOCK_SIZE,
						  sizeof(struct taskinfo));
		taskmax = taskblock++ + PROCBLOCK_SIZE;
    }

    /* initialization */
    bzero(p->name,sizeof(p->name));
#if 0
    if (p->name != NULL)
    {
		free(p->name);
		p->name = NULL;
    }
#endif

    return p;
}

void free_task(struct taskinfo *task)
{
    task->next = freelist;
    freelist = task;
}



void read_one_task_info(pid_t pid,taskinfo * tinfo,machine_s * MACHINE)
{
    int fd,len;
    char buffer[4096],* p;
    long page_size;
    page_size = sysconf(_SC_PAGESIZE) >> 10;
    /* use_cpu,sys_cpu,task_cpu*/
    unsigned long time_interval;
    unsigned long outime;                    /** user mode jiffies                                         **/
    unsigned long ostime;                    /** kernel mode jiffies                                       **/
    unsigned long ocutime;                   /** user mode jiffies with childs                             **/
    unsigned long ocstime;                   /** kernel mode jiffies with childs                           **/
    unsigned long ototal;
    static int flags = 0;
    /* 	
     *  	* 我们假设在调用get_task_info之前，已经调用了更新值得函数。
	 * 注意这里的时间是滴答数吗？I don't know.
	 */
    time_interval = MACHINE->CPU->new_total[0] - MACHINE->CPU->old_total[0];
    outime  = tinfo->utime;
    ostime  = tinfo->stime;
    ocstime = tinfo->cstime;
    ocutime = tinfo->cutime;
    ototal  = tinfo->total;
    {

        /***********read /proc/<pid>/stat******************/
        sprintf(buffer,"/proc/%d/stat",pid);
        if((fd = open(buffer,O_RDONLY)) < 0)
        {
            printf("Not such proccess\n"); 
            exit(-1);
        }
        len = read(fd, buffer, sizeof(buffer) -1 );
        close(fd);
        buffer[len] = '\0';
		
        tinfo->pid = pid;
		p = buffer;
		p = strchr(p, '(')+1;			/* skip pid */
		
		char *q = strrchr(p, ')');
		int len = q-p;
        if(flags == 0)
        {            
		    if (len >= sizeof(tinfo->name))
			    len = sizeof(tinfo->name)-1;
		    memcpy(tinfo->name, p, len);
		    tinfo->name[len] = 0;
            flags = 1;
        }
		p = q+1;
		

		p = skip_ws(p);
		switch (*p++)
		{
		  case 'R': tinfo->state = 1; break;
		  case 'S': tinfo->state = 2; break;
		  case 'D': tinfo->state = 3; break;
		  case 'Z': tinfo->state = 4; break;
		  case 'T': tinfo->state = 5; break;
		  case 'W': tinfo->state = 6; break;
		}
		
		p = skip_token(p);				/* skip ppid */
		p = skip_token(p);				/* skip pgrp */
		p = skip_token(p);				/* skip session */
		p = skip_token(p);				/* skip tty */
		p = skip_token(p);				/* skip tty pgrp */
		p = skip_token(p);				/* skip flags */
		p = skip_token(p);				/* skip min flt */
		p = skip_token(p);				/* skip cmin flt */
		p = skip_token(p);				/* skip maj flt */
		p = skip_token(p);				/* skip cmaj flt */
		
		tinfo->utime = strtoul(p, &p, 10);			/* utime */
		tinfo->stime = strtoul(p, &p, 10);			/* stime */

		tinfo->cutime = strtoul(p, &p, 10);			/*  cutime */
		tinfo->cstime = strtoul(p, &p, 10);			/*  cstime */

		p = skip_token(p);							/* skip priority */
		p = skip_token(p);							/* skip nice */

		p = skip_token(p);							/* skip timeout */
		p = skip_token(p);							/* skip it_real_val */
		tinfo->start_time = strtoul(p, &p, 10);		/* get start_time */

		tinfo->vsize = bytetok(strtoul(p, &p, 10));	    /* vsize KB*/
		tinfo->rss = (strtoul(p, &p, 10) * page_size);	/* rss KB*/
    }
    /***********read /proc/<pid>/statm*****************/
	{
	    sprintf(buffer,"/proc/%d/statm",pid);
        fd = open(buffer,O_RDONLY);
        len = read(fd,buffer,sizeof(buffer)-1);
        close(fd);
        buffer[len] = '\0';
        p = buffer;
        p = skip_token(p);               /* skip first */
        p = skip_token(p);               /* skip second */
        tinfo->shares = strtoul(p,&p,10) * page_size;
	}
#define LIKE_TOP
#ifdef  LIKE_TOP 
	tinfo->total = tinfo->utime
		         + tinfo->stime;
#else
	tinfo->total = tinfo->utime
		+ tinfo->stime
		+ tinfo->cutime
		+ tinfo->cstime;
#endif
#if 0
        printf("oldcputime:%lu,newcputime:%lu\n",CPU.old_total[0],CPU.new_total[0]);
        printf("tinfo->utime:%lu,outime:%lu\n",tinfo->utime,outime);
        printf("tinfo->stime:%lu,ostime:%lu\n",tinfo->stime,ostime);
        printf("tinfo->total:%lu,ototal:%lu\n",tinfo->total,ototal);
#endif
    tinfo->use_mem = 1.0 * tinfo->rss / MACHINE->MEMINFO->mem_total;

    if(tinfo->use_mem < 0.0001)
    {
        tinfo->use_mem = 0.0;
    }
	if((tinfo->use_cpu = (tinfo->utime - outime)/(double)time_interval) < 0.0001)
	{
		tinfo->use_cpu = 0.0;
	}
	if((tinfo->sys_cpu = (tinfo->stime - ostime)/(double)time_interval) < 0.0001)
	{
		tinfo->sys_cpu = 0.0;
	}
	if((tinfo->task_cpu = (tinfo->total - ototal)/(double)time_interval) < 0.0001)
	{
		tinfo->task_cpu = 0.0;
	}

}
#if 0
void get_task_info(void)
{
	hash_item_pid * hi;
	hash_pos        pos;
	taskinfo * 		task;
	pid_t 			pid;
	/* mark all hash table entries as not seen */
        hi = hash_first_pid(ptable, &pos);
        while (hi != NULL)
        {
			((struct taskinfo *)(hi->value))->state = 0;
			hi = hash_next_pid(&pos);
        }
        pactives = 0;
		{
		DIR * dir = opendir("/proc");
		struct dirent * ent;
		while((ent = readdir(dir)) != NULL)
		{
			if(!isdigit(ent->d_name[0]))
				continue;
			pid = atoi(ent->d_name);
			/* hash table look up*/
			task = hash_lookup_pid(ptable,pid);
			/* we get NULL,so we create a new item */
			if(task == NULL)
			{
				task = new_task();
				task->pid = pid;
				task->total  = 0;
				task->utime  = 0;
				task->stime  = 0;
				task->cstime = 0;
				task->cutime = 0;
				hash_add_pid(ptable,pid,(void *)task);
			}
			read_one_task_info(pid,task);
			/* continue on if this isn't really a process */
			if (task->state == 0)
				continue;
			
			/* reset linked list (for threads) */
			task->next = NULL;
			
			++pactives;
		}
		closedir(dir);
		taskptr = (taskinfo **)realloc(taskptr,sizeof(taskinfo *) * pactives);
		taskinfo ** p = taskptr;
		hi = hash_first_pid(ptable, &pos);
		while (hi != NULL)
		{
			task = (struct taskinfo *)(hi->value);
			if (task->state == 0)
			{
			/* dead entry */
				hash_remove_pos_pid(&pos);
				free_task(task);
			}
			else
			{
				*p++ = task;
			}
			hi = hash_next_pid(&pos);
		}
		//qsort(taskptr,pactives,sizeof(taskinfo *),task_compares[sort_mode]);
	}
}
#endif
#define MAX_CLOS 255
const char * format_task_info(taskinfo * task)
{
	static char format[MAX_CLOS];
	snprintf(format,sizeof(format),
	"%5d\t%5d\t%5d\t%5d\t%5.2f\t%5.2f\t%5.2f\t%5.2f\t%s",
	task->pid,
	(task->vsize),
	(task->rss),
	(task->shares),
    100.0 * task->use_mem,
	100 * task->use_cpu,
	100 * task->sys_cpu,
	100 * task->task_cpu,
	task->name
	);
	return (format);
}
pid_t get_process_by_name(const char * name)
{
    pid_t pid;
    char buffer[_POSIX_PATH_MAX] = {0};
	DIR * dir = opendir("/proc");
	struct dirent * ent;
    int fd = 0;
    int len = 0;
	while((ent = readdir(dir)) != NULL)
	{
		if(!isdigit(ent->d_name[0]))
				continue;
		pid = atoi(ent->d_name);
        sprintf(buffer,"/proc/%d/cmdline",pid);
        //why don't check, beacuse we are use /proc filesytem
        fd  = open(buffer,O_RDONLY); 
        len = read(fd,buffer,sizeof(buffer)-1);
        close(fd);
        buffer[len] = '\0';
        if(strcmp(buffer,name) == 0)
        {
            return pid;
        } 
    }
    closedir(dir);
    return -1;
}


pid_t get_process_by_id(pid_t test_pid)
{
    pid_t pid;
	DIR * dir = opendir("/proc");
	struct dirent * ent;
	while((ent = readdir(dir)) != NULL)
	{
		if(!isdigit(ent->d_name[0]))
				continue;
		pid = atoi(ent->d_name);
        // look up
        if(test_pid == pid)
        {
            return pid;
        }
    }
    closedir(dir);
    return -1;
}
#if 0
void view_task_info(void)
{
	//hash_item_pid * hi;
	taskinfo * task;
	get_task_info();
	for(int i = 0; i < pactives;++i)
	{
		task = taskptr[i];
		printf("%s\n",format_task_info(task));
	}
        /* hash_pos pos;
	hi = hash_first_pid(ptable, &pos);
        while (hi != NULL)
        {
	    task = (struct taskinfo *)(hi->value);
    	    printf("%s\n",format_task_info(task));
	    hi = hash_next_pid(&pos);
        } */
}
#endif
