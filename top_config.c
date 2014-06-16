#include "top_config.h"
static inline char * skip_var_name(char * p)
{
    while(!isspace(p[0]) && p[0] != '=' && p[0] != '\0')
    {
        p++;
    }
    return p;
}
static inline char * skip_opeartor(char * p)
{
    while((isspace(p[0])||p[0] == '=') && p[0] != '\0')
    {
        p++;
    } 
    return p;
}
static inline char * strupr(char * p)
{
    char * q = p;
    while(*p)
    {
        *p = toupper(*p);
        p++;
    }
    return q;
}
int top_read_config_file(const char * file_name,config_s * config)
{
    FILE * fp = NULL;
    char buf[BUFSIZ] = {0};
    char * p, *q;
    char * pname = NULL;
    int count = 0;
    int flag = 0; 
    config->pid_flag = config->pname_flag = config->time_flag = 0;
    if((fp = fopen(file_name,"r")) == NULL)
    {
        perror("can't open config file");
        return -1;
    }
    while(1)
    {
        fgets(buf,BUFSIZ,fp);
        if(feof(fp))
        {
            break;
        }
        count = strlen(buf);
        p = buf;
        //printf("count=%d\t%s",count,buf);
        //skip whitespace
        while(count > 0 &&isspace(p[0]))
        {
            count--;
            p++;
        }
        //printf("count=%d\t%s",count,buf+i);
        if(p[0] == '#')
        {
            continue;
        }
        //printf("count=%d\t%s",count,p);
        q = skip_var_name(p);
        pname = malloc(q-p+1);
        strncpy(pname, p, q-p );
        p = q;
        p = skip_opeartor(p);
        //printf("%s\n",p);
        pname = strupr(pname);
        //switch 判断var_name属于哪一类.
        if(strcmp(pname,"PID") == 0)
        {
            if(flag == 0)
            {
                config->pid = atoi(p);  
                config->pid_flag = 1;
                flag = 1;
            }
            else
            {
                config->conflict_flag = 1;
            }
        }
        else if(strcmp(pname,"PNAME") == 0)
        {
            if(flag == 0)
            {
                q = skip_var_name(p);
                config->pname = malloc(q-p+1);
                strncpy(config->pname,p,q-p);
                config->pname_flag = 1;
                flag = 1;
            }
            else
            {
                config->conflict_flag = 1;
            }
        }
        else if(strcmp(pname,"TIME") == 0)
        {
            config->time_flag = 1;
            config->time = atoi(p);
        }
        free(pname);

        
    }
    return 0;
}
