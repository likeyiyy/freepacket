/*************************************************************************
	> File Name: dispaly.c
	> Author: likeyi
	> Mail: likeyi@sina.com 
	> Created Time: Tue 10 Jun 2014 10:05:03 PM CST
 ************************************************************************/

#include "includes.h"
#include <ncurses.h>
#define WORKER_SET_SIZE 3
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
static void display_gen(generator_group_t * generator_group)
{
    static uint64_t new = 0;
    static uint64_t old = 0;
    pool_t * pool;
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
    //wprintw(win->win,"All Byte add:%llu,%llu Mbps\n",(new-old),(new-old)/(1024*1024)*8);

    old = new;
    new = 0;

}
static void display_generator(window_t * win,generator_group_t * generator_group)
{
    static uint64_t new = 0;
    static uint64_t old = 0;
    pool_t * pool;
    for(int i = 0; i < generator_group->numbers; ++i)
    {
        pool = generator_group->generator[i].pool; 
        wprintw(win->win,"pool free:%u total_send_byte:%llu \ndrop pempty total :%lu,drop qfull total: %lu\n",
                pool->free_num,
                generator_group->generator[i].total_send_byte,
                generator_group->generator[i].drop_pempty_total,
                generator_group->generator[i].drop_qfull_total
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
         wprintw(win->win,"Queue size:%lu pool free:%u recv_byte:%lu MB\n",
                 parser_group->parser[i].queue->length,
                 parser_group->parser[i].pool->free_num,
                 parser_group->parser[i].total >> 20);
    }
    wprintw(win->win,"\n\n\n");
    wrefresh(win->win);
}
static void display_manager(window_t * win,manager_group_t * manager_group)
{
    for(int i = 0; i < manager_group->length; ++i)
    {
        
        wprintw(win->win,"Queue Size:%lu pool free:%u hash_count:%d\n",
               manager_group->manager[i].queue->length,
               manager_group->manager[i].session_pool->free_num,
               hash_count(manager_group->manager[i].ht));
    }
    wprintw(win->win,"\n\n\n");
    wrefresh(win->win);
}

/*
* Fuction: Display the queue length pool length and so on
* Author:  likeyi
* Date:    2014年6月10日13:55:59
* */
void sys_dispaly(generator_group_t * generator_group,
                parser_group_t * parser_group,
                manager_group_t * manager_group)
{
#ifdef INTEL_PLATFORM
    //screen_init();
	int i = 0;
    while(1)
    {
#if 0
        display_generator(&window[0],generator_group);

        display_parser(&window[1],parser_group);

        display_manager(&window[2],manager_group);

#else
        display_gen(generator_group);
#endif
        usleep(1000 * 1000);
		if(i++ == 5)
		{
			exit(0);
		}
    }
    //endwin();
#endif
}

