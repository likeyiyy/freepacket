/*************************************************************************
	> File Name: dispaly.c
	> Author: likeyi
	> Mail: likeyi@sina.com 
	> Created Time: Tue 10 Jun 2014 10:05:03 PM CST
 ************************************************************************/

#include "includes.h"
#define WORKER_SET_SIZE 3
#include <ncurses.h>
typedef struct 
{
    int height;
    int width;
    int startx;
    int starty;
    WINDOW * win;
}window_t;
static window_t window[WORKER_SET_SIZE];
static void screen_init()
{
    initscr();
   for(int i = 0; i < WORKER_SET_SIZE; ++i) 
   {
       window[i].height = LINES - 1;
       window[i].width  = COLS / WORKER_SET_SIZE;
       window[i].starty = 0;
       window[i].startx = i * COLS / WORKER_SET_SIZE;
       window[i].win    = newwin(
               window[i].height,
               window[i].width,
               window[i].starty,
               window[i].startx
                );
       scrollok(window[i].win,1);
   }
}
static void display_generator(window_t * win,generator_group_t * generator_group)
{
    static uint64_t new = 0;
    static uint64_t old = 0;
    free_pool_t * pool;
    for(int i = 0; i < generator_group->numbers; ++i)
    {
        pool = generator_group->generator[i].pool; 
		wprintw(win->win,"[%u] ",generator_group->generator[i].alive);
        wprintw(win->win,"pool free:%u total_send_byte:%llu \nEmpty: %lu\n",
				 (pool->enqueue_count + 1024 - pool->dequeue_count) % 1024,
                generator_group->generator[i].total_send_byte,
                generator_group->generator[i].drop_pempty_total
                //generator_group->generator[i].drop_qfull_total
                );
        /* FIXME Only */
        new += generator_group->generator[i].total_send_byte;
    }
    wprintw(win->win,"All Byte add:%llu,%llu Mbps\n",(new-old),(new-old)/(1024*1024)*8);
    old = new;
    new = 0;
    wprintw(win->win,"\n\n\n");
    wrefresh(win->win);
}
static void display_parser(window_t * win, parser_group_t *  parser_group)
{
    for(int i = 0; i < parser_group->numbers; ++i)
    {
		wprintw(win->win,"[%u] ",parser_group->parser[i].alive);
         wprintw(win->win,"Queue size:%u pool free:%u \n",
				 (parser_group->parser[i].queue->enqueue_count + 1024 - parser_group->parser[i].queue->dequeue_count) % 1024,
                 parser_group->parser[i].pool->enqueue_count - parser_group->parser[i].pool->dequeue_count);
                 //parser_group->parser[i].total >> 20);
    }
	for(int i = 0; i < 16; ++i)
	{
		wprintw(win->win,"%d ",ghash_view[i]);
	}
    wprintw(win->win,"\n\n\n");
    wrefresh(win->win);
}
static void display_manager(window_t * win,manager_group_t * manager_group)
{
    for(int i = 0; i < manager_group->numbers; ++i)
    {
        
		wprintw(win->win,"[%u] ",manager_group->manager[i].alive);
        wprintw(win->win,"Queue Size:%lu pool free:%u hash_count:%d\n",
			   manager_group->manager[i].queue->enqueue_count - manager_group->manager[i].queue->dequeue_count,
               manager_group->manager[i].session_pool->free_num,
               hash_count(manager_group->manager[i].ht));
    }
    wprintw(win->win,"\n\n\n");
    wrefresh(win->win);
}
static void display_gen(generator_group_t * generator_group)
{
    static uint64_t new = 0;
    static uint64_t old = 0;
    int drop_flag = 0;
    for(int i = 0; i < generator_group->numbers; ++i)
    {
        /* FIXME Only */
		if(generator_group->generator[i].drop_pempty_total || generator_group->generator[i].drop_qfull_total )
		{
			drop_flag = 1;
		}

        new += generator_group->generator[i].total_send_byte;
    }
	if((new-old) != 0)
		printf("SPEED: %lu :Mbit/s,DROP: %d \n",(new-old)/(1024*128),drop_flag);
    else
		printf("SPEED: %lu :Mbit/s,DROP: %d \n",(new-old)/(1024*128),drop_flag);
        
    old = new;
    new = 0;
}

void display_gpm(generator_group_t * generator_group,
                parser_group_t * parser_group,
                manager_group_t * manager_group,int maximum)
{

    static uint64_t new = 0;
    static uint64_t old = 0;
    free_pool_t * pool;
	uint64_t send_bytes[36] = {0};
	/*
	 * Read first and not changed.
	 * */
	for(int i = 0; i < generator_group->numbers; i++)
	{
		send_bytes[i] = generator_group->generator[i].total_send_byte;
        new += send_bytes[i];
	}

	for(int i = 0; i < maximum; i++)	
	{
		if(i < generator_group->numbers)
		{
        	pool = generator_group->generator[i].pool; 
			printf("[%12u] ",generator_group->generator[i].alive);
        	printf("pool free:%4u Bytes :%12lu              ",
				 (pool->enqueue_count - pool->dequeue_count),
				 send_bytes[i]
                );
        	/* FIXME Only */
		}
		else
		{
			printf("                                                               ");
		}

		if(i < parser_group->numbers)
		{
			pool = parser_group->parser[i].pool;
			printf("[%12u] ",parser_group->parser[i].alive);
        	printf("Queue :%4u Pool :%5u     \t\t",
				 (parser_group->parser[i].queue->enqueue_count - parser_group->parser[i].queue->dequeue_count),
                 pool->enqueue_count - pool->dequeue_count);
		}
		else
		{
			printf("                                                         ");
		}
		if(i < manager_group->numbers)
		{
			printf("[%4u] ",manager_group->manager[i].alive);
        	printf("Queue :%5u Pool :%4u Hash :%4u",
               manager_group->manager[i].queue->enqueue_count - manager_group->manager[i].queue->dequeue_count,
               manager_group->manager[i].session_pool->free_num,
               hash_count(manager_group->manager[i].ht));
		}
		printf("\n");
	}
    printf("All Byte add:%lu,%lu Mbps\n",(new-old),(new-old)/(1024*1024)*8);
    old = new;
    new = 0;
}

/*
* Fuction: Display the queue length pool length and so on
* Author:  likeyi
* Date:    2014年6月10日13:55:59
* */
void sys_display(generator_group_t * generator_group,
                parser_group_t * parser_group,
                manager_group_t * manager_group)
{
	int state = 0;
	int maximum = generator_group->numbers;
	if(maximum < parser_group->numbers)
	{
		maximum = parser_group->numbers;
	}
	if(maximum < manager_group->numbers)
	{
		maximum = manager_group->numbers;
	}
	while(1)
	{
		switch(state)
		{
			case 0:
			if(global_config->screen_display == 1)
			{
				state = 2;
			}
			else if(global_config->screen_display == 0)
			{
				state = 1;
			}
			break;

			case 1:
			if(global_config -> screen_display == 1)
			{
				state = 2;	
				printf("\n");
			}
			else
			{
        		display_gen(generator_group);
			}
			break;

			case 2:
			if(global_config -> screen_display == 0)
			{
				state = 1;
			}
			else
			{
				display_gpm(generator_group,parser_group,manager_group,maximum);
			}
			break;
		}
		read_config_simple(CONFIG_FILE,global_config);	
        usleep(1000 * 1000);
	}
}

void flow_display(flow_item_t * flow)
{
	struct in_addr in;
	in.s_addr = flow->upper_ip;
    printf("upper ip:%s \t",inet_ntoa(in));
	in.s_addr = flow->lower_ip;
    printf("lower ip:%s \n",inet_ntoa(in));
    printf("upper port:%d \t",ntohs(flow->upper_port));
    printf("lower port:%d \n",ntohs(flow->lower_port));
}

