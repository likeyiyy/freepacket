#include "cpuinfo.h"
#include "top_config.h"
#include <stdio.h>
#include <stdlib.h>
static int CPU_NUM = 0;
void get_cpuinfo(cpu_time_s * cpu)
{
    FILE * fp; 
    char buffer[4096];
    if((fp = fopen("/proc/stat","r")) == NULL)
    {   
         printf("error open stat\n");
         exit(-1);
    }   
    //get cpu info
    for(int i = 0; i < CPU_NUM+1;i++)
    {
        fgets(buffer,sizeof(buffer),fp);    
        //get each time
	    sscanf(buffer,"%s%lu%lu%lu%lu%lu%lu%lu%lu%lu",
        cpu->new_cpu_info[i].cpuname,
	    &cpu->new_cpu_info[i].user,
	    &cpu->new_cpu_info[i].nice,
	    &cpu->new_cpu_info[i].system,
	    &cpu->new_cpu_info[i].idle,
	    &cpu->new_cpu_info[i].iowait,
	    &cpu->new_cpu_info[i].irq,
	    &cpu->new_cpu_info[i].softirq,
	    &cpu->new_cpu_info[i].stealstolen,
	    &cpu->new_cpu_info[i].guest);
      //compute total time
    cpu->new_total[i] = 
	    cpu->new_cpu_info[i].user
	    +cpu->new_cpu_info[i].nice
	    +cpu->new_cpu_info[i].system
	    +cpu->new_cpu_info[i].idle
	    +cpu->new_cpu_info[i].iowait
	    +cpu->new_cpu_info[i].irq
	    +cpu->new_cpu_info[i].softirq
	    +cpu->new_cpu_info[i].stealstolen
	    +cpu->new_cpu_info[i].guest;
    }
    fclose(fp);
}
void update_cpuinfo(cpu_time_s * cpu)
{ 
    //exchange cpu info
    for(int i = 0; i < CPU_NUM+1;i++)
    {
        cpu->old_total[i] = cpu->new_total[i];
        cpu->old_cpu_info[i] = cpu->new_cpu_info[i];
    }
}
void cpuinfo_view(cpu_time_s * cpu)
{
    //compute cpu rate
    for(int i = 0; i < CPU_NUM+1;i++)
    {
        
	cputime_t period = cpu->new_total[i] - cpu->old_total[i];
	if(period == 0)
		period = 1;
	printf("%s\t%2.2f%%user  %2.2f%%nice  %2.2f%%system  %2.2f%%idle  %2.2f%%io  %2.2f%%irq  %2.2f%%softirq  %2.2f%%st  %2.2f%%guest\n",
                cpu->new_cpu_info[i].cpuname,
		100.0 * (cpu->new_cpu_info[i].user        - cpu->old_cpu_info[i].user )       / period,	
		100.0 * (cpu->new_cpu_info[i].nice        - cpu->old_cpu_info[i].nice )       / period,	
		100.0 * (cpu->new_cpu_info[i].system      - cpu->old_cpu_info[i].system)      / period,	
		100.0 * (cpu->new_cpu_info[i].idle        - cpu->old_cpu_info[i].idle)        / period,	
		100.0 * (cpu->new_cpu_info[i].iowait      - cpu->old_cpu_info[i].iowait)      / period,	
		100.0 * (cpu->new_cpu_info[i].irq         - cpu->old_cpu_info[i].irq)         / period,	
		100.0 * (cpu->new_cpu_info[i].softirq     - cpu->old_cpu_info[i].softirq)     / period,	
		100.0 * (cpu->new_cpu_info[i].stealstolen - cpu->old_cpu_info[i].stealstolen) / period,	
		100.0 * (cpu->new_cpu_info[i].guest       - cpu->old_cpu_info[i].guest)       / period	
	);
    
    }
    //at there sleep or not is OK
		
}
int findstr(char* src, char* s)
{
        char *ptr=src, *p=s;    //定义两个指针 
        char *ptr2=src+strlen(src), *prev=NULL;    //ptr2为src的末位置指针 
        int len=strlen(s), n=0;        //子串的长度和计数器 
        for(;*ptr;ptr++)    //循环整个串 
        {
            if(ptr2-ptr<len)    //如果一开始子串就大于src,则退出 
                break;
            for(prev=ptr;*prev==*p;prev++,p++)    //寻找第一个相等的位置,然后从此位置开始匹配子串 
            {
               if(*(p+1)==0)                       //如果已经到了子串的末尾 
                {
                    n++;    //自增 
                    p=s;    //重新指向子串 
                    break;//退出 
                                                                                                                  
                }
                                             
            }
            p = s;
       }
       return n;
}
int init_cpuinfo(cpu_time_s * cpu)
{
    int fd = 0;
    int len = 0;
    char buf[4096];
    fd = open("/proc/stat",O_RDONLY);
    len = read(fd,buf,sizeof(buf));
    close(fd);
    buf[len] = '\0';
    int times = findstr(buf,"cpu");
    CPU_NUM = times - 1;
    printf("CPU_NUM:%d\n",CPU_NUM);
    cpu->new_cpu_info = calloc(1,sizeof(cpuinfo_s) * times);
    cpu->old_cpu_info = calloc(1,sizeof(cpuinfo_s) * times);
    cpu->new_total = calloc(1,sizeof(cputime_t) * times);
    cpu->old_total = calloc(1,sizeof(cputime_t) * times);
    if((cpu->new_cpu_info == NULL)||
            (cpu->old_cpu_info == NULL)||
            (cpu->new_total == NULL)||(cpu->old_total == NULL))
    {
        printf("can not alloc cpu info struct\n");
        exit(-1);
    }
    return 0;
}
