/*************************************************************************
	> File Name: lossratio.h
	> Author: likeyi
	> Mail: likeyi@sina.com 
	> Created Time: Mon 30 Jun 2014 05:01:14 PM CST
 ************************************************************************/

#ifndef LOSSRATIO_H
#define LOSSRATIO_H

#include <stdint.h>
typedef struct
{
    uint64_t drop_cause_generator_pool_empty;
    uint64_t drop_cause_parser_queue_full;
    uint64_t drop_cause_parser_pool_empty;
    uint64_t drop_cause_unsupport_protocol;

    uint64_t drop_cause_empty_payload;
    uint64_t drop_cause_manager_pool_empty;
    uint64_t send_total;
}drop_t;

extern drop_t * global_loss;

void * start_ipc_server(void * arg);

void * start_ipc_client(void * arg);

double drop_ratio(drop_t * new, drop_t * old);

#endif
