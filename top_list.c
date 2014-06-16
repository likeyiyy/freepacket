/**
# Author: likeyi
# Email: likeyiyy@sina.com
=========================================================================
 File Name: list.c
 Description: (C)  2014-03  findstr
   
 Edit History: 
   2014-03-12    File created.
=========================================================================
**/
#include "top_config.h"
#include "top_list.h"
list_s * LIST;
static inline node_s * make_node(void * value)
{
    node_s * node = (node_s *)malloc(sizeof(node_s));
    node->value = value;
    node->next  = NULL;
    return node;
}
list_s * create_list()
{
    list_s * list = (list_s *)malloc(sizeof(list_s));
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}
void add_node(list_s * list, void * value)
{
   node_s * node;
   node = make_node(value);
   if(list->head == NULL) 
   {
        list->head = node;
        list->tail = node;
        node -> next = NULL;
   }
   else
   {
        list->tail->next = node;
        list->tail = node;
        node -> next = NULL; 
   }
   list->size++;
}
int del_node(list_s * list,void ** value)
{
    node_s * node;
    if(list->head == NULL)
        return -1;
    *value = list->head->value;
    node = list->head;
    if(node->next == NULL)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        list->head = node->next;
    }
    free(node);     //it only free node,not free taskinfo
    list->size--;
    return 0; 
}
void iteration_list(list_s * list,void (*iter_func)(void * value))
{
   node_s * node;
   node = list->head;
   while(node)
   {
        iter_func(node->value);
        node = node->next;
   }
}
int is_empty(list_s * list)
{
    return list->size == 0 ? 1:0;
}
