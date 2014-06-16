#include "top_config.h"
#include "cpuinfo.h"
#include "meminfo.h"
#include "hash.h"
#include "top_list.h"
#include "use_file.h"
#include "taskinfo.h"
const char * config_error = "ERROR:You can only specify PID or process name,not both two specified";
cpu_time_s CPU;
meminfo_s MEMINFO;
hash_table *ptable;
int processid;
char * process_name;
list_s * LIST;
pthread_t tid;
struct timeval sleeptime = {1,0};
machine_s MACHINE = {&CPU,&MEMINFO};
config_s config,config_cmd,*config_ptr;

const char * help = "        -f config file name \n\
        -i period time to update \n\
        -p process pid\n\
        -n process name\n\
        -h help";
static inline int us_sleep(struct timeval sleeptime)
{
    struct timeval tv;
    tv = sleeptime;
    return select(0,NULL,NULL,NULL,&tv);
}

int ttop(int argc,char ** argv)
{
    for(int i = 0; i < argc; ++i)
    {
        printf("argv[%d] =[]%s[]\n",i,argv[i]);
    }
	/***********get opt ***************/
	int opt;
    taskinfo task = {0};
    taskinfo * ptask;
    processid = 1;
    int len = 0;
    int pid_mod    = 0;
    int pname_mod  = 0;
    char * config_file_name = NULL; 
    int config_file_flag = 0;
    // get the config arg at first
    while((opt = getopt(argc,argv,"f:n:i:p:h")) != -1)
	{
		switch(opt)
		{
            case 'f':
                config_file_name = optarg; 
                top_read_config_file(config_file_name,&config); 
                config_file_flag = 1;
                break;
			case 'i': 
				config_cmd.time = atoi(optarg);
                config_cmd.time_flag = 1;
				break;
			case 'p':
                if(pname_mod == 1)
                {
                    config_cmd.conflict_flag = 1;
                }
                else
                {
				    config_cmd.pid = atoi(optarg);
                    pid_mod = 1;
                    config_cmd.pid_flag = 1;
                }
				break;
            case 'n':
                if(pid_mod == 1)
                {
                    config_cmd.conflict_flag = 1;
                }
                else
                {
				    len = strlen(optarg);
                    config_cmd.pname = malloc(len+1);
                    strncpy(config_cmd.pname,optarg,len);
                    pname_mod = 1;
                    config_cmd.pname_flag = 1;
                }
				break;
            case 'h':
                printf("%s\n",help);
                exit(0);
                
			default:
				break;
		}
	}
    //假如有配置文件，那么就以配置文件为先，忽略掉其他参数的内容。
    if(config_file_flag == 1)
    {
        config_ptr = & config;
    }
    else
    {
        config_ptr = &config_cmd;
    }
    //假如配置文件里面，同时指定了PID和Pname
    if(config_ptr->conflict_flag == 1)
    {
        printf("ERROR,YOU can not assign both PID and PNAME\n");
        exit(-1);
    }
    else
    {
        //假如只指定了pname,检验Pname的有效性。
        if(config_ptr->pname_flag)
        {
            config_ptr->pid = get_process_by_name(config_ptr->pname);
        }
        else if(config_ptr->pid_flag) //假如指定了PID,检验pid的有效性
        {
            config_ptr->pid = get_process_by_id(config_ptr->pid);
        }
        else
        {
            config_ptr->pid = 1;
        }
        //假如找不到，则说明没有这个进程，报错。
        if(config_ptr->pid < 0)
        {
            printf("Not such process\n");
            exit(-1);
        }
        if(config_ptr->time_flag)
        {
            sleeptime.tv_sec  = config_ptr->time / 1000;
            sleeptime.tv_usec = config_ptr->time % 1000 * 1000;
        }
    }
    free(config.pname);
    free(config_cmd.pname);
    ptable = hash_create(100);
    LIST   = create_list();
    get_meminfo(&MEMINFO);
    init_cpuinfo(&CPU);
    if(pthread_create(&tid,NULL,write_to_file,NULL) == -1)
    {
        printf("create pthread error\n");
        exit(-1);
    }
	printf("TIMESTAMP\t    PID\tVIRT\tRES\tSHARE\t%%MEM\t%%USER\t%%SYS\t%%CPU\tCOMMAND\n");
	while(1)
	{
		get_cpuinfo(&CPU);
        read_one_task_info(config_ptr->pid,&task,&MACHINE);  
		update_cpuinfo(&CPU);
        gettimeofday(&task.timestamp,NULL);
        ptask = new_task();
        memcpy(ptask,&task,sizeof(task));
        add_node(LIST,ptask);
        printf("%lu.%lu ",task.timestamp.tv_sec,task.timestamp.tv_usec);
		printf("%s\n",format_task_info(&task));
        us_sleep(sleeptime);
	}
}
