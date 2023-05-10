#ifndef METHOD_H
#define METHOD_H
#include <string>
using std::string;

#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/uio.h>
#include "../http_conn/http_const.h"
//方法基类
class method
{
    
public:
    //生产响应报文的函数组，被组装响应报文的函数调用 
    bool add_response( const char* format, ... )
    {
        if( m_write_idx >= WRITE_BUFFER_SIZE )
        {
            return false;
        }
        va_list arg_list;
        va_start( arg_list, format );
        int len = vsnprintf( m_write_buf + m_write_idx, WRITE_BUFFER_SIZE - 1 - m_write_idx, format, arg_list );
        if( len >= ( WRITE_BUFFER_SIZE - 1 - m_write_idx ) )
        {
            return false;
        }
        m_write_idx += len;
        va_end( arg_list );
        return true;
    }
    //加入请求行
    bool add_status_line( int status, const char* title )
    {
        return add_response( "%s %d %s\r\n", "HTTP/1.1", status, title );
    }
    //响应报文消息体长度首部
    bool add_content_length( int content_len )
    {
        return add_response( "Content-Length: %d\r\n", content_len );
    }
    //空行
    bool add_blank_line()
    {
        return add_response( "%s", "\r\n" );
    }
    //消息体
    bool add_content( const char* content )
    {
        return add_response( "%s", content );
    }
    //获取请求行解析结果，读缓冲区位置，写缓冲区位置
    void init_method(char* url,char* version,char* write_buf,int write_idx)
    {
        m_url=url;
        m_version=version;
        m_write_buf=write_buf;
        m_write_idx=write_idx;
    }
    virtual ~method()=default;
//解析首部行，处理消息体，处理请求报文，组建响应报文的接口

    virtual void prase_headers(char* text)=0;
    virtual HTTP_CODE prase_content(char* text)=0;
    virtual HTTP_CODE do_request()=0;
    virtual bool process_write( HTTP_CODE ret )=0;
protected:

    //请求行的解析结果
    char* m_url;//客户请求的目标文件名
    char* m_version;//HTTP版本号
    
    //写缓冲区
    char* m_write_buf;
    int m_write_idx;
};
#endif