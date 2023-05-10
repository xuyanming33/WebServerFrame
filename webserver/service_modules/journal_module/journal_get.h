#ifndef JOURNAL_GET_H
#define JOURNAL_GET_H
#include "jounal_authentication_method.h"
//日记业务的get方法
class journal_get:public journal_method
{
public:
    //get方法解析首部行接口
    void prase_headers(char* text)override
    {
        prase_only_get_headers(text);
        prase_Authorization_header(text);
    }
    


    //日记业务的get方法请求报文消息体的处理方法（依赖请求行和首部行的解析结果）
    HTTP_CODE prase_content(char* text)override
    {
        return HTTP_CODE::GET_REQUEST;
    }

    //日记业务的get方法解析结果的处理
    HTTP_CODE do_request()override
    {
        auto tmp=search_file();
        if(tmp!=GET_REQUEST)
        return tmp;
        //打开文件并且映射到内存
        int fd = open( m_real_file, O_RDONLY );
        m_file_address = ( char* )mmap( 0, m_file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0 );
        close( fd );
        return HTTP_CODE::FILE_REQUEST;
    }

    

    //日记业务的get方法响应报文组建
    bool process_write( HTTP_CODE ret )override
    {
        switch ( ret )
        {
            case INTERNAL_ERROR://内部错误
            {
                add_status_line( 500, error_500_title );
                add_headers( strlen( error_500_form ) );
                if ( ! add_content( error_500_form ) )
                {
                    return false;
                }
                break;
            }
            case BAD_REQUEST://请求报文错误
            {
                
                 add_status_line( 400, error_400_title );
                 //增加一种用户不存在的情况,发送特殊的原因短语
                if(!exist_client)
                {
                   add_headers( strlen( error_400_form1 ) );
                }
                else
                {   
                    add_headers( strlen( error_400_form ) );
                }
                    
                if ( ! add_content( error_400_form ) )
                {
                    return false;
                }
                
                    break;
            }
            case NO_RESOURCE://文件不存在
            {
                add_status_line( 404, error_404_title );
                add_headers( strlen( error_404_form ) );
                if ( ! add_content( error_404_form ) )
                {
                    return false;
                }
                break;
            }
            case FORBIDDEN_REQUEST://文件权限不够
            {
                add_status_line( 403, error_403_title );
                add_headers( strlen( error_403_form ) );
                if ( ! add_content( error_403_form ) )
                {
                    return false;
                }
                break;
            }
            case FILE_REQUEST://找到文件
            {
                add_status_line( 200, ok_200_title );
                if ( m_file_stat.st_size != 0 )
                {
                    add_headers( m_file_stat.st_size );
                    m_iv[ 0 ].iov_base = m_write_buf;
                    m_iv[ 0 ].iov_len = m_write_idx;
                    m_iv[ 1 ].iov_base = m_file_address;
                    m_iv[ 1 ].iov_len = m_file_stat.st_size;
                    m_iv_count = 2;
                    return true;
                }
                else
                {
                    const char* ok_string = "<html><body></body></html>";
                    add_headers( strlen( ok_string ) );
                    if ( ! add_content( ok_string ) )
                    {
                        return false;
                    }
                }
            }
            default:
            {
                return false;
            }
        }

        m_iv[ 0 ].iov_base = m_write_buf;
        m_iv[ 0 ].iov_len = m_write_idx;
        m_iv_count = 1;
        return true;
    } 



private:
    //解析结果,get方法用通用的首部已经足够，不要增加首部
   
    //处理请求和组装响应报文需要的
    char* m_file_address;//文件映射到内存的位置
    struct iovec m_iv[2];//要发送的缓存区
    int m_iv_count;//要发送的缓冲区个数

    //get方法独有首部行的解析方法（接口）
    void prase_only_get_headers(char* text)
    {
        return;
        
    }

public:
    virtual ~journal_get()=default;
};
#endif