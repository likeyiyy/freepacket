/*************************************************************************
	> File Name: sim_top.c
	> Author: likeyi
	> Mail: likeyiyy@sina.com 
	> Created Time: Tue 13 May 2014 04:07:01 PM CST
 ************************************************************************/

#include "includes.h"
extern manager_set_t * manager_set;
extern generator_set_t * generator_set;
extern parser_set_t * parser_set;
extern config_t * config;
int top_argc = 0;
char ** top_argv;


static char* itoa(int num,char*str,int radix)
{/*索引表*/
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned unum;/*中间变量*/
    int i=0,j,k;
/*确定unum的值*/
    if(radix==10&&num<0)/*十进制负数*/
    {
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else unum=(unsigned)num;/*其他情况*/
/*转换*/
 do{
     str[i++]=index[unum%(unsigned)radix];
     unum/=radix;

 }while(unum);
str[i]='\0';
/*逆序*/
if(str[0]=='-')
    k=1;/*十进制负数*/
else 
    k=0;
char temp;
for(j=k;j<=(i-1)/2;j++)
 {
     temp=str[j];
     str[j]=str[i-1+k-j];
     str[i-1+k-j]=temp;

 }
return str;
}
int main(int argc, char ** argv)
{
    pid_t pid;
    if((pid = fork()) < 0)
    {
        printf("fork error\n");
        exit(0);
    }
    else if(pid == 0)
    {
        init_manager_set(10);
        init_parser_set(5);
        init_generator_set(10);
        while(1)
        {
            sleep(10);
        }
        /* loop here */
        //sys_dispaly(generator_set,parser_set,manager_set);
    }
    else
    {
        top_argc = 3;
        top_argv = malloc(sizeof(char *) * 3);
        if(top_argv == NULL)
        {
            printf("error alloc\n");
        }

        top_argv[0] = calloc(strlen("ttop") + 1,sizeof(char *));
        if(top_argv[0] == NULL)
        {
            printf("error alloc\n");
        }
        strcpy(top_argv[0],"ttop");


        top_argv[1] = calloc(strlen("-p") + 1,sizeof(char *));
        if(top_argv[1] == NULL)
        {
            printf("error alloc\n");
        }
        strcpy(top_argv[1],"-p");


        top_argv[2] = calloc(10,sizeof(char *));
        if(top_argv[2] == NULL)
        {
            printf("error alloc\n");
        }
        itoa(pid,top_argv[2],10);

        ttop(top_argc,top_argv);
    }
}
