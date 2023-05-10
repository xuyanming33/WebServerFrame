#ifndef GET_SERVICE_H
#define GET_SERVICE_H
#include "../service_modules/journal_module/journal_service.h"
#include "../http_conn/http_const.h"
#include "../base_class/method.h"
#include <string.h>
#include <memory>
#include <unordered_map>
class get_service
{
public:
    get_service():services_table(new std::unordered_map<std::string,method*>){}
    ~get_service()=default;
public:
    //注册业务 返回值表明插入是否成功
    bool add_service(METHOD m_method,std::string user_agent,method* serv)
    {
        auto s=services_table->insert(std::make_pair(method_translate(m_method)+user_agent,serv));
        return s.second;
    }

    //查找业务
    method* find_service(METHOD m_method,std::string user_agent)
    {
        auto a=services_table->find(method_translate(m_method)+user_agent);
        if(a==services_table->end())
        {
            return nullptr;
        }
        else
        {
            auto s=(a->second);//从查找表里拷贝一个对象返回
            return s;
        }  
    }

private:
    std::unique_ptr<std::unordered_map<string,method*>> services_table;//业务查找表
    string method_translate(METHOD method)
    {
        if(method==GET)
        return "GET";
        else if(method==POST)
        return "POST";
        else if(method==HEAD)
        return "HEAD";
        else if(method==PUT)
        return "PUT";
        else if(method==DELETE)
        return "DELETE";
        else if(method==TRACE)
        return "TRACE";
        else if(method==OPTIONS)
        return "OPTIONS";
        else if(method==CONNECT)
        return "CONNECT";
        else if(method==PATCH)
        return "PATCH";
        else   
        return "GET";
    }
};
#endif
//GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH 