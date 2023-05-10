#ifndef JOURNAL_PUT_H
#define JOURNAL_PUT_H
#include "jounal_authentication_method.h"
//日记业务的put方法
class journal_put:public journal_method
{
public:
    void prase_headers(char* text)override
    {
        prase_only_put_headers(text);
        prase_Authorization_header(text);
    }
    

    HTTP_CODE prase_content(char* text)override
    {
        if(creat(m_real_file,S_IRUSR|S_IWUSR))
        {
            file_created=open(m_real_file,O_RDWR);
            write(file_created,text,m_content_length);
            return HTTP_CODE::GET_REQUEST;
        }
        else 
        return FORBIDDEN_REQUEST;
    }


    HTTP_CODE do_request()override
    {
        return GET_REQUEST;
    }


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
            default:
            return false;
        }
    }


private:


    void prase_only_put_headers(char* text)
    {
        return;
    }
    int file_created;


public:
    journal_put(){};
    virtual ~journal_put()=default;
};
#endif