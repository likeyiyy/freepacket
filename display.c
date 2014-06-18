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
static void display_generator(window_t * win,generator_set_t * generator_set)
{
    static uint64_t new = 0;
    static uint64_t old = 0;
    pool_t * pool;
    for(int i = 0; i < generator_set->numbers; ++i)
    {
        pool = generator_set->generator[i].pool; 
        wprintw(win->win,"pool free:%u total_send_byte:%llu \n",
                pool->free_num,
                generator_set->generator[i].total_send_byte 
                );
        /* FIXME Only */
        new += generator_set->generator[i].total_send_byte;
    }
    wprintw(win->win,"All Byte add:%llu,%llu MB\n",(new-old),(new-old)/(1024*1024));
    old = new;
    new = 0;
    wprintw(win->win,"\n\n\n");
    wrefresh(win->win);
}
static void display_parser(window_t * win, parser_set_t *  parser_set)
{
    for(int i = 0; i < parser_set->numbers; ++i)
    {
         wprintw(win->win,"Queue size:%lu pool free:%u recv_byte:%llu\n",
                 parser_set->parser[i].queue->length,
                 parser_set->parser[i].pool->free_num,
                 parser_set->parser[i].total
                );
    }
    wprintw(win->win,"\n\n\n");
    wrefresh(win->win);
}
static void display_manager(window_t * win,manager_set_t * manager_set)
{
    for(int i = 0; i < manager_set->length; ++i)
    {
        
        wprintw(win->win,"Queue Size:%lu pool free:%u hash_count:%d\n",
               manager_set->manager[i].queue->length,
               manager_set->manager[i].session_pool->free_num,
               hash_count(manager_set->manager[i].ht));
    }
    wprintw(win->win,"\n\n\n");
    wrefresh(win->win);
}
/*
* Fuction: Display the queue length pool length and so on
* Author:  likeyi
* Date:    2014年6月10日13:55:59
* */
void sys_dispaly(generator_set_t * generator_set,
                parser_set_t * parser_set,
                manager_set_t * manager_set)
{
    screen_init();
    while(1)
    {
        display_generator(&window[0],generator_set);

        display_parser(&window[1],parser_set);

        display_manager(&window[2],manager_set);

        usleep(1000 * 1000);
    }
    endwin();
}

