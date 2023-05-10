#ifndef JOUNAL_AUTHENTICATION_METHOD_H
#define JOUNAL_AUTHENTICATION_METHOD_H
#include "../../base_class/method.h"
//#include "../../basic_components/threadsafe_data_structure/threadsafe_lookup_table.h"
#include <string>
class journal_method:public method
{
public:
    virtual void prase_headers(char* text)=0;
    virtual HTTP_CODE prase_content(char* text)=0;
    virtual HTTP_CODE do_request()=0;
    virtual bool process_write( HTTP_CODE ret )=0;
    virtual ~journal_method()=default;
public:
    //首部行解析函数，基本认证所需的首部行
    //解析首部行的函数
    void prase_Authorization_header(char* text)
    {
        if ( strncasecmp( text, "Authorization:", 14 ) == 0 )
        {
            text+=14;
            text+=strspn(text," \t");//定位用户名字段第一个字符
            user_id=text;
            user_password=strpbrk(user_id," \t");//定位到用户名字段后一位并设置为空字符
            *user_password++='\0';
            user_password+=strspn(user_password," \t");//定位到密码字段第一位
        }
        else if ( strncasecmp( text, "Connection:", 11 ) == 0 )
        {
            text += 11;
            text += strspn( text, " \t" );
            if ( strcasecmp( text, "keep-alive" ) == 0 )
            {
                m_linger = true;
            }
        }
        else if ( strncasecmp( text, "Host:", 5 ) == 0 )
        {
            
            text += 5;
            text += strspn( text, " \t" );
            m_host = text;
        }  
        else if ( strncasecmp( text, "Content-Length:", 15 ) == 0 )
        {
            text += 15;
            text += strspn( text, " \t" );
            m_content_length = atol( text );
        }
    }
    //日记业务响应报文组建的工具函数
        bool add_linger()
        {
            return add_response( "Connection: %s\r\n", ( m_linger == true ) ? "keep-alive" : "close" );
        }
        bool add_headers( int content_len )
        {
            add_content_length(content_len);
            add_linger();
            add_blank_line();
            return true;
        }
    //登录
    bool login()
    {
        return true;
        /*if(authentication=check_userid_and_password())
        {
            return true;
        }
        else
        return false;*/
    }

    //注册
    bool registration()
    {
        return true;
        /*if(authentication=check_userid_and_password())
        {
            add_userid_and_password();
        }
        else
        return false;*/
    }    
    
   

protected:
//查找文件
    HTTP_CODE search_file()
    {
        //先登录
        if(!login())
        {
            return BAD_REQUEST;//没有登录成功
        }
        strcpy( m_real_file, doc_root );
        int len = strlen( doc_root );
        strncpy( m_real_file + len, m_url, FILENAME_LEN - len - 1 );
        if ( stat( m_real_file, &m_file_stat ) < 0 )//找文件没找到
        {
            return HTTP_CODE::NO_RESOURCE;
        }

        if ( ! ( m_file_stat.st_mode & S_IROTH ) )//权限不够
        {
            return HTTP_CODE::FORBIDDEN_REQUEST;
        }

        if ( S_ISDIR( m_file_stat.st_mode ) )//验证是否为目录
        {
            return HTTP_CODE::BAD_REQUEST;
        }
        else
        return GET_REQUEST;
    }
protected:
//文件
    char m_real_file[FILENAME_LEN];//根目录加文件名
    struct stat m_file_stat;//记录文件状态的结构体
//用户
    char* user_id;//用户名
    char* user_password;//用户密码
    bool registration_or_login;//false为注册，true为登录，应该默认为登录
    bool exist_client;//false代表客户不存在，true代表客户存在

    bool authentication;//认证结果

    bool m_linger;//是否保持长连接
    char* m_host;//主机名
    int m_content_length;//消息体长度，默认为0
    //User-Agent字段由任务类直接解析

    //并查集
    //static threadsafe_lookup_table tslu_table;

private:
    //查询数据库中有没有符合的用户名以及正确的密码
    bool check_userid_and_password()
    {
        return true;/*
        //确保用户名和密码有效
        if(user_id&&user_password)
        {
            string str1,str2;
            if((str1=user_password)==(str2=(tslu_table.find_if_exist(user_id))))
            {
                exist_client=true;
                return true;
            }
            else return false;
        }
        else return false;*/
        
    }

    //向数据库中添加用户名和密码
    bool add_userid_and_password()
    {
        return true;/*
        char* s=nullptr;
        tslu_table.insert_data(user_id,s);
        if(s)
        {
            return false;
        }
        else return true;*/
    
    }

};
#endif