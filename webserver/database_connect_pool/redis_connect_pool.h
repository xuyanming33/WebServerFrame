/* #include "hiredis/hiredis.h"
#include "threadsafe_queue.h"
#include <strings.h>
#include <stdio.h>

class redis_connect_pool
{
public:

    //构造函数负责建立connect_count数量的连接
    redis_connect_pool()
    {
        redis_connects=new redisContext*[connect_count];
        auto s=redis_connects;
        for(int i=0;i<connect_count;i++)
        {
            while((*s=redisConnect(ip,port))->err||(s==nullptr))
            {
                redisFree(*s);
            };
            q.push(*s);
            s++;
        }
    }

    //析构函数要负责关闭连接
    ~redis_connect_pool()
    {
        //redisFree(redisContext *c)
         auto s=redis_connects;
        for(int i=0;i<connect_count;i++)
        {
            redisFree(*s);
            s++;
        }
        delete[]redis_connects;
    }

private:
    const char *ip="127.0.0.1";
    int port=6379;
    const static int connect_count;//连接数量，应该与线程数量一致比较好
    redisContext **redis_connects;//指向redis连接的数组指针
    threadsafe_queue<redisContext*> q;//线程安全队列

    //

public:

    //给用户调用的接口，负责发送请求返回请求
    bool redis_send_message(int reply_type,const char *format, ...)
    {
        
        auto c=q.wait_and_pop();
        va_list arg_list;
        va_start( arg_list, format );
        redisReply* s;
        //这个是存储用户名和函数操作，返回的是状态码
        if(reply_type==1)
        {
            while(!(s=(redisReply*)redisCommand(*c, format, arg_list ))&&(s->type==REDIS_REPLY_STATUS));//发送报文至有成功回应为止
            if(strcasecmp(s->str,"OK")==0)
            {
                return true;
            }
            else 
            return false;
        }
        //错误码
        else if(reply_type==2)
        {
            
        }
        va_end( arg_list );
        return s;
    }

}; */
/**连接数据库*/
/* redisContext *redisConnect(const char *ip, int port);
/**发送命令请求*/
//void *redisCommand(redisContext *c, const char *format, ...);
//void *redisCommandArgv(redisContext *c, int argc, const char **argv, const size_t *argvlen);
//void redisAppendCommand(redisContext *c, const char *format, ...);
//void redisAppendCommandArgv(redisContext *c, int argc, const char **argv, const size_t *argvlen);
/*释放资源*/
//void freeReplyObject(void *reply);
//void redisFree(redisContext *c); 