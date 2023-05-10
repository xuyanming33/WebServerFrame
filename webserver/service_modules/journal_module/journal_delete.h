#ifndef JOURNAL_DELETE_H
#define JOURNAL_DELETE_H
#include "jounal_authentication_method.h"
//日记业务delete方法
class journal_delete:journal_method
{
public:
   
    void prase_headers(char* text)override
    {
        prase_only_delete_headers(text);
        prase_Authorization_header(text);
    }
    

    HTTP_CODE prase_content(char* text)override//delete方法不处理消息体
    {
        return HTTP_CODE::GET_REQUEST;
    }

    HTTP_CODE do_request()override
    {
        auto tmp=search_file();
        if(tmp!=GET_REQUEST)
        return tmp;
        //删除文件
        if(!remove(m_real_file))
        return FORBIDDEN_REQUEST;
        return GET_REQUEST;
    }

    bool process_write( HTTP_CODE ret )override
    {
        switch ( ret )
        {
            case GET_REQUEST://删除成功
            {
                add_status_line( 200, ok_200_title );
            }
            case BAD_REQUEST://报文错误
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
            case FORBIDDEN_REQUEST://权限不足
            {
                add_status_line( 403, error_403_title );
                add_headers( strlen( error_403_form ) );
                if ( ! add_content( error_403_form ) )
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


    //get方法独有首部行的解析方法（接口）
    void prase_only_delete_headers(char* text)
    {
        return;
    }




public:
    virtual ~journal_delete()=default;
};
#endif