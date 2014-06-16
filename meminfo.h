#ifndef L_MEMINFO_H
#define L_MEMINFO_H
typedef struct meminfo
{
	unsigned long mem_total;
	unsigned long mem_free;
	unsigned long mem_buffers;
	unsigned long mem_cached;
	unsigned long swap_total;
	unsigned long swap_free;
}meminfo_s;
void view_meminfo(meminfo_s * meminfo_ptr);
void get_meminfo(meminfo_s * meminfo_ptr); 
#endif
