/*************************************************************************
	> File Name: lossratio.c
	> Author: likeyi
	> Mail: likeyi@sina.com 
	> Created Time: Mon 30 Jun 2014 05:01:06 PM CST
 ************************************************************************/

#include "includes.h"

drop_t global_loss_body;

drop_t * global_loss = &global_loss_body;

#define UNIX_DOMAIN "/tmp/UNIX.domain"

int create_ipc_client()
{
    int connect_fd;
    static struct sockaddr_un srv_addr;
    int ret;
    connect_fd=socket(PF_UNIX,SOCK_STREAM,0);
    if(connect_fd<0)
    {
        perror("cannot create communication socket");
        return 1;
    }   
    srv_addr.sun_family=AF_UNIX;
    strcpy(srv_addr.sun_path,UNIX_DOMAIN);
    ret=connect(connect_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
    if(ret==-1)
    {
        perror("cannot connect to the server");
        close(connect_fd);
        return 1;
    }
    return connect_fd;

}
int create_ipc_server()
{
    int listen_fd;
    struct sockaddr_un srv_addr;
    int ret;
    listen_fd=socket(PF_UNIX,SOCK_STREAM,0);
    if(listen_fd<0)
    {
        perror("cannot create communication socket");
        return 1;
    }  
    //set server addr_param
    srv_addr.sun_family=AF_UNIX;
    strncpy(srv_addr.sun_path,UNIX_DOMAIN,sizeof(srv_addr.sun_path)-1);
    unlink(UNIX_DOMAIN);
    //bind sockfd & addr
    ret=bind(listen_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
    if(ret==-1)
    {
        perror("cannot bind server socket");
        close(listen_fd);
        unlink(UNIX_DOMAIN);
        return 1;
    }
    //listen sockfd 
    ret=listen(listen_fd,1);
    if(ret == -1)
    {
        perror("cannot listen the client connect request");
        close(listen_fd);
        unlink(UNIX_DOMAIN);
        return 1;
    }
    return listen_fd;

}
static int get_update_speed(const char * buf)
{
    int i = 0;
    while(!isdigit(*(buf+i)))
    {
        i++;
    }
    int speed = atoi(buf+i);
    return speed;
}
void * process_ipc_request(void * arg)
{
    int com_fd = * (int *)arg;
    char recv_buf[1024]; 
    char send_buf[1024];
    char * sync_req = "SYN_REQ";
    strcpy(recv_buf,sync_req);
    write(com_fd,recv_buf,sizeof(recv_buf));
    read(com_fd,recv_buf,sizeof(recv_buf));
    //int i = 0;
    /* 返回syn_ok的同时，设置速度 */
    if(strstr(recv_buf,"SYN_OK") != NULL);
    {
        int speed = get_update_speed(recv_buf);
        while(1)
        {
            //sprintf(send_buf,"hello world %d times",i++);
            memcpy(send_buf,global_loss,sizeof(drop_t));
            int result = write(com_fd,send_buf,sizeof(drop_t));
            if(result < 0)
            {
                pthread_exit(NULL);
            }
            usleep(speed * 1000);
        }
    }
    close(com_fd);
    pthread_exit(NULL);
}

void * start_ipc_server(void * arg)
{
    //socklen_t clt_addr_len;
    int com_fd;
    //int ret;
    //int i;
    socklen_t len;
    struct sockaddr_un clt_addr;
    //struct sockaddr_un srv_addr;
    int listen_fd = create_ipc_server();
    //have connect request use accept
    len    = sizeof(clt_addr);

    while(1)
    {
        com_fd = accept(listen_fd,(struct sockaddr*)&clt_addr,&len);
        if(com_fd < 0)
        {
            perror("cannot accept client connect request");
            close(listen_fd);
            continue;
        }
        pthread_t tid;
        pthread_create(&tid,NULL,process_ipc_request,&com_fd);


    }
    close(listen_fd);
    unlink(UNIX_DOMAIN);
    pthread_exit(NULL);
}

void * start_ipc_client(void * arg)
{
    int connect_fd;
    //int ret;
    char snd_buf[1024];
    char recv_buf[1024];
    //int i;
    connect_fd = create_ipc_client();
    int update_speed = *(int *)arg;

    /*
     * 1,接受同步请求。
     * */
    char * sync_req = "SYN_REQ";
    memset(snd_buf,0,1024);
    read(connect_fd,snd_buf,sizeof(snd_buf));
    if(strcmp(snd_buf,sync_req) == 0)
    {
        /*
         * 发送速率请求
         * */
        memset(snd_buf,0,1024);
        sprintf(snd_buf,"SYN_OK:SET_SPEED:%d",update_speed);
        write(connect_fd,snd_buf,sizeof(snd_buf));

        int com_fd = connect_fd;
        while(1)
        {
            int num=read(com_fd,recv_buf,sizeof(recv_buf));
            if(num == 0)
            {
                close(com_fd);
                pthread_exit(NULL);
            }
            memcpy(global_loss,recv_buf,num);
#if 1
            printf("%lu,%lu,%lu,%lu,%lu,%lu,%lu\n",
                  global_loss->drop_cause_generator_pool_empty,
                  global_loss->drop_cause_parser_queue_full,
                  global_loss->drop_cause_parser_pool_empty,
                  global_loss->drop_cause_unsupport_protocol,
                  global_loss->drop_cause_empty_payload,
                  global_loss->drop_cause_manager_pool_empty,
                  global_loss->send_total);
#endif
            //printf("Message from server (%d)) :[%s]\n",num,recv_buf);  
        }
    }
    close(connect_fd);
    pthread_exit(NULL);
}


double drop_ratio(drop_t * new, drop_t * old)
{
    uint64_t old_drop,new_drop;
    old_drop = new_drop = 0;

    uint64_t old_total,new_total;

    old_drop += old->drop_cause_generator_pool_empty;
    old_drop += old->drop_cause_parser_queue_full;
    old_drop += old->drop_cause_parser_pool_empty;
    old_drop += old->drop_cause_unsupport_protocol;
    old_drop += old->drop_cause_empty_payload;
    old_drop += old->drop_cause_manager_pool_empty;
    
    new_drop += new->drop_cause_generator_pool_empty;
    new_drop += new->drop_cause_parser_queue_full;
    new_drop += new->drop_cause_parser_pool_empty;
    new_drop += new->drop_cause_unsupport_protocol;
    new_drop += new->drop_cause_empty_payload;
    new_drop += new->drop_cause_manager_pool_empty;

    new_total = new->send_total;
    old_total = old->send_total;

    printf("new_drop:[%lu],old_drop:[%lu],new_total:[%lu],old_total:[%lu]\n",
            new_drop,
            old_drop,
            new_total,
            old_total);
    double rate;
    rate = 1.0 * (new_drop - old_drop)/(new - old);
    return rate;
}
