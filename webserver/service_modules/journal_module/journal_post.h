#ifndef JOURNAL_POST_H
#define JOURNAL_POST_H
#include "jounal_authentication_method.h"
//日记业务的post方法
class journal_post:public journal_method
{
public:
    //post方法解析首部行接口
    void prase_headers(char* text)override
    {
        prase_only_get_headers(text);//post独有的首部行解析
        prase_Authorization_header(text);//继承通用方法的首部行解析
    }
    

    //日记业务的post方法请求报文消息体的处理方法（依赖请求行和首部行的解析结果）
    HTTP_CODE prase_content(char* text)override
        {
            if(m_content_length<=0)
            {
                return BAD_REQUEST;
            }
            else if ( strncasecmp( text, "registration", 12 ) == 0 )
            {
                registration_or_login=false;
                return HTTP_CODE::GET_REQUEST;
            }
            else if ( strncasecmp( text, "login", 5 ) == 0 )
            {
                registration_or_login=true;
                return HTTP_CODE::GET_REQUEST;
            }
            else
                return HTTP_CODE::BAD_REQUEST;

            
        }

    //日记业务的post方法解析结果的处理
    HTTP_CODE do_request()override
    {
        if(registration_or_login==false)//注册
        {
            if(registration())
            return GET_REQUEST;//登录成功
            else
            return BAD_REQUEST;//登录失败
        }
        else if(login())//登录
        {
            return GET_REQUEST;//登录成功
        }
        else
        return BAD_REQUEST;//登录失败

        
    }

    //日记业务的post方法响应报文的组建
    bool process_write( HTTP_CODE ret )override
    {
        switch ( ret )
        {
            case GET_REQUEST://登录或注册成功
            add_status_line( 200, ok_200_title );
            case BAD_REQUEST://登录或注册失败
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
            default:
            {
                add_status_line( 500, error_500_title );
                add_headers( strlen( error_500_form ) );
                if ( ! add_content( error_500_form ) )
                {
                    return false;
                }
                break;
            }
        }
    }

private:
    //post方法独有首部的解析
    void prase_only_get_headers(char* text)
    {
        return;//没有需要增加的首部行
    }


public:
    virtual ~journal_post()=default;
};
#endif