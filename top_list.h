/**
# Author: likeyi
# Email: likeyiyy@sina.com
=========================================================================
 File Name: list.h
 Description: (C)  2014-03  findstr
   
 Edit History: 
   2014-03-12    File created.
=========================================================================
**/
#ifndef L_LIST_H
#define L_LIST_H
typedef struct node_
{
    void * value;
    struct node_ * next;
}node_s;
typedef struct list_
{
    int size;
    node_s * head;
    node_s * tail;
}list_s;
list_s * create_list();
void add_node(list_s * list,void * value);
int del_node(list_s * list,void ** value);
void iteration_list(list_s * list,void (*iter_func)(void * value));
int is_empty(list_s * list);
#endif
