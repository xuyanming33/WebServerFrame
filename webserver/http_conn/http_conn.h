#ifndef HTTP_CONN_H
#define HTTP_CONN_H

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
#include <unordered_map>
#include <vector>

#include "http_const.h"

//业务文件
#include "../basic_components/get_service.h" 
//加入业务
#include "../service_modules/journal_module/journal_service.h"//日记业务

class http_conn
{
public:
    http_conn(){}
    ~http_conn(){}

public:

    //初始化任务类的客户数据(给主函数(主线程)调用的接口)
    void init( int sockfd, const sockaddr_in& addr );

    //关闭连接
    void close_conn( bool real_close = true );

    //处理任务(给工作线程调用的接口)
    void process();

    //给主函数(主线程)的接口，读至不可读，写至不可写的ET模式下非阻塞读和写，I/O单元完全在主线程，工作线程处理读完成事件，注册写就绪事件
    bool read();
    bool write();

private:

    void init();

    //get方法解析报文和响应报文函数
    HTTP_CODE process_read();
    bool process_write( HTTP_CODE ret );
    
    //get方法解析报文的工具函数
    HTTP_CODE parse_request_line( char* text );//解析请求行
    HTTP_CODE parse_headers( char* text );//解析首部行
    HTTP_CODE parse_content( char* text );//处理消息体
    HTTP_CODE do_request();

    char* get_line() { return m_read_buf + m_start_line; }//定位解析行
    LINE_STATUS parse_line();//预解析出一行(这个函数是为了确定接下来要解析的一行的状态)

    //get方法响应报文的根据函数
    void unmap();
    /*bool add_response( const char* format, ... );
    bool add_content( const char* content );
    bool add_status_line( int status, const char* title );
    bool add_headers( int content_length );
    bool add_content_length( int content_length );
    bool add_linger();
    bool add_blank_line();*/

public:
    
    //任务类的注册表和客户数量统计
    static int m_epollfd;
    static int m_user_count;

private:

    //客户数据
    int m_sockfd;//套接字
    sockaddr_in m_address;//客户IP地址

    //读缓冲区数据
    char m_read_buf[ READ_BUFFER_SIZE ];//读缓冲区
    int m_read_idx;//缓冲区第一个空槽
    int m_start_line;//解析行起始位置
    int m_checked_idx;//当前解析的位置
    
    //写缓冲区数据
    char m_write_buf[ WRITE_BUFFER_SIZE ];//写缓冲区
    int m_write_idx;//写缓冲区待发送的字节数

    //解析方法
    CHECK_STATE m_check_state;//状态机
    METHOD m_method;//方法
    
    //get方法解析结果
    char m_real_file[ FILENAME_LEN ];//目标文件完整路径，doc_root+m_url,doc_root是根目录，m_url是文件名
    char* m_url;//文件名
    char* m_version;//协议版号，HTTP/1.1
    char* m_host;//主机名
    int m_content_length;//请求报文消息体长度
    bool m_linger;//HTTP请求是否要保持连接
    
    //get方法响应报文
    char* m_file_address;//目标文件被mmap到内存中的位置
    struct stat m_file_stat;//目标文件状态
    struct iovec m_iv[2];//响应报文请求行+首部行和消息体的集中写
    int m_iv_count;//集中写的内存块数量

public:
    //设置写缓冲区,业务类写好响应报文后调用
    void set_write_buf(int write_idx)
    {
        m_write_idx=write_idx;
    }

private:
    //业务管理
    get_service get_serv;//用于管理业务的对象
    method* serv;//处理本次请求的业务对象
    char* user_agent;//User-Agent首部
    /*//向任务类注册业务
    void add_service(char* user_agent,string serv)
    {
        std::string name(user_agent);
        services.insert(make_pair(name,serv));
    }
    //从任务类删除业务
    void delete_service(char* user_agent)
    {
        std::string name(user_agent);
        services.erase(name);
    }
    //用method和User-Agent首部确定业务
    void locate_service(char* user_agent)
    {
        std::string name(user_agent);
        //std::unordered_map<std::string,service*>::const_iterator 
        auto s=(services.equal_range(name)).first;
        //没找到就返回空指针
        if(s==services.cend())
        {
            serv="non-existent";
        }
        serv=(*s).second;
    }
    //创建一个动态数组存储所有的业务类对象
    //std::shared_ptr<std::vector<service>> service_vector(new std::vector<service>());
private:
    static std::unordered_map<std::string,std::string> services;
    
    std::shared_ptr<service> work_service;//处理本次请求的业务对象*/
};












#endif


/*
解析报文函数(process_read)的设计：
主状态机会跟踪当前状态：解析请求行(CHECK_STATE_REQUESTLINE)/解析首部行(CHECK_STATE_HEADER)/处理消息体(parse_content),解析报文是先调用(parse_line)预解析出一行，
预解析结果(LINE_STATUS)是成功(LINE_OK)/失败(LINE_BAD)/未完成(LINE_OPEN),
再调用请求行解析函数(parse_request_line)/首部行解析函数(parse_headers)

*/



/*
改进：
1.缓冲区能否采用环形的结构，或者采用分散读的方法充分利用缓冲区
2.能不能在一个连接上批量处理http请求报文(流水线)，但下的模式仍旧是线性的，得先处理好一个报文发出响应报文之后才能处理新的报文。
3.关于报文出错的处理，报文出错可能在行预解析的时候出现，也可能在解析请求行，首部行的时候发现，一旦报文本身出错，当下我们是没有
  能力解决这个问题的，只能中止这个解析的过程发送响应报文告诉客户端报文错误，等发送完响应报文后关闭这个连接。这里的问题在于还没
  把报文发送过去就把连接关闭了。
*/