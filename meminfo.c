/**
=========================================================================
 Author: findstr
 Email: findstr@sina.com
 File Name: meminfo.c
 Description: (C)  2014-03  findstr
   
 Edit History: 
   2014-03-04    File created.
=========================================================================
**/
#include "meminfo.h"
#include "top_config.h"
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
void get_meminfo(meminfo_s * meminfo_ptr)
{
	int fd;
	int len;
	char buffer[4096];
	char * p;
	if((fd = open("/proc/meminfo",O_RDONLY)) < 0)
	{
		perror("open meminfo error");
		exit(-1);
	}
	len = read(fd,buffer,sizeof(buffer)-1);
	buffer[len] = '\0';
	p = buffer;
	p = skip_token(p);
	meminfo_ptr->mem_total 		= strtoul(p,&p,10);
	p = skip_token(p);
	p = skip_token(p);
	meminfo_ptr->mem_free 		= strtoul(p,&p,10);
	p = skip_token(p);
	p = skip_token(p);
	meminfo_ptr->mem_buffers	= strtoul(p,&p,10);
	p = skip_token(p);
	p = skip_token(p);
	meminfo_ptr->mem_cached 	= strtoul(p,&p,10);
	int i = 0; 
	for(i = 0; i < 29; ++i)
	{
		p = skip_token(p);
	}
	meminfo_ptr->swap_total 	= strtoul(p,&p,10);
	p = skip_token(p);
	p = skip_token(p);
	meminfo_ptr->swap_free 		= strtoul(p,&p,10);
}
void meminfo_view(meminfo_s * meminfo_ptr)
{
	get_meminfo(meminfo_ptr);
	printf("Mem:\t%luk total,\t%luk used,\t%luk free,\t%lu buffers\n",
			meminfo_ptr->mem_total,
			meminfo_ptr->mem_total - meminfo_ptr->mem_free,
			meminfo_ptr->mem_free,
			meminfo_ptr->mem_buffers
			);
	printf("Swap:\t%luk total,\t%luk used,\t%luk free,\t%lu buffers\n",
			meminfo_ptr->swap_total,
			meminfo_ptr->swap_total - meminfo_ptr->swap_free,
			meminfo_ptr->swap_free,
			meminfo_ptr->mem_cached
			);
}
