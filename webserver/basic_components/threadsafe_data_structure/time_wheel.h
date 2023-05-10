#ifndef TIME_WHEEL_H
#define TIME_WHEEL_H
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <netinet/in.h>

static const int BUFFER_SIZE=64;
static const int time_circle=1;
static const int inwheel=256;
static const int outwheel=64;
static const int outwheelnum=4;
static const int mods[]={inwheel,inwheel*outwheel,inwheel*outwheel^2,inwheel*outwheel^3,inwheel*outwheel^4};
class tw_timer;
/* 把客户和定时器设置成一一对应关系 */
struct client_data
{
    sockaddr_in address;
    int sockfd;
    tw_timer* timer;
};

class time_wheel;
/*定时器类*/
class tw_timer
{
    friend class time_wheel;
public:
    tw_timer(int ts):next(NULL),prev(NULL),time_slot(ts)
    {for(int i=0;i<5;i++)
    {
        all_wheel_slot[i]=0;
    }
    };

public:
    void (*cb_func)(client_data*);
    client_data* user_data;
private:    
    int time_wheelID;
    int time_slot;
    int all_wheel_slot[5];
    tw_timer* next;
    tw_timer* prev;
};

/*时间轮*/
class time_wheel
{
public:
    time_wheel()
    {
        for(int i=0;i<5;i++)
        {cur_slot[i]=0;}
        for(int i=0;i<inwheel;i++)
        {
            inwheel_slot[i]=NULL;
        }
        for(int i=0;i<outwheelnum;i++)
        {
            for(int j=0;j<outwheel;j++)
            {
                alloutwheel[i][j]=NULL;
            }
        }
    };
    ~time_wheel()
    {
        for(int i=0;i<inwheel;i++)
        {
            tw_timer* tmp=inwheel_slot[i];
            while (tmp)
            {
                inwheel_slot[i]=tmp->next;
                delete tmp;
                tmp=inwheel_slot[i];
            }
            
        }
        for(int i=0;i<outwheelnum;i++)
        {
            for(int j=0;j<outwheel;j++)
            {
                tw_timer* tmp=alloutwheel[i][j];
                while(tmp)
                {
                    alloutwheel[i][j]=tmp->next;
                    delete tmp;
                    tmp=alloutwheel[i][j];
                }
            }
        }
    }
//数据成员
private:
    tw_timer* inwheel_slot[inwheel];
    tw_timer *alloutwheel[outwheelnum][outwheel];
    static const int SI=time_circle;
    int cur_slot[5];
//接口
public:
    tw_timer* add_timer(int timeout);
    void del_timer(tw_timer* timer);
    tw_timer* update_timer(tw_timer* timer,int timeout);
    void tick();
    
private:
    tw_timer* inwheel_add(int slot,tw_timer* timer)
    {
        if(inwheel_slot[slot]=NULL)
        {
            timer->time_slot=slot;
            timer->prev=NULL;
            timer->next=NULL;
            inwheel_slot[slot]=timer;
            return timer;
        }
        else
        timer->time_slot=slot;
        tw_timer* tmp=inwheel_slot[slot]->next;
        timer->next=tmp;
        tmp->prev=timer;
        timer->prev=inwheel_slot[slot];
        inwheel_slot[slot]->next=timer;
        return timer;
    }

    tw_timer* outwheel_add(int wheelID,int slot,tw_timer* timer)
    {
        if(alloutwheel[wheelID][slot]=NULL)
        {
            timer->time_slot=slot;
            timer->next=NULL;
            timer->prev=NULL;
            alloutwheel[wheelID][slot]=timer;
            return timer;
        }
        else
        timer->time_slot=slot;
        tw_timer* tmp=alloutwheel[wheelID][slot]->next;
        timer->next=tmp;
        tmp->prev=timer;
        timer->prev=alloutwheel[wheelID][slot];
        alloutwheel[wheelID][slot]->next=timer;
        return timer;
    }

    int locate_wheel_and_update_slots(int timeout,tw_timer*timer)
    {   
        for(int i=0;i<5;i++)
        {
            if(i==0)
            timer->all_wheel_slot[i]=((timeout%mods[i]==0)?mods[i]:(timeout%mods[i]));
            else
            int j=(timeout/mods[i-1]);//have problem,why "j" can't be used?
            timer->all_wheel_slot[i]=(((timeout/mods[i-1])%mods[i]==0)?mods[i]:((timeout/mods[i-1])%mods[i]));
        }
        int wheelID=0;
        if(0<timeout<mods[0]+1)
        wheelID=1;
        else if(mods[0]<timeout<mods[1]+1)
        wheelID=2;
        else if(mods[1]<timeout<mods[2]+1)
        wheelID=3;
        else if(mods[2]<timeout<mods[3]+1)
        wheelID=4;
        else if(mods[3]<timeout<mods[4]+1)
        wheelID=5;
        else 
        wheelID=0;
        return wheelID;
    }

    tw_timer* del_timer_from_wheellist(tw_timer* timer)
    {
    if(timer->prev==NULL)
        {
        if(timer->time_wheelID==1)
           {
            inwheel_slot[timer->time_slot]=timer->next;
            inwheel_slot[timer->time_slot]->prev=NULL;
            return timer;
           }
        else
           {
            alloutwheel[timer->time_wheelID][timer->time_slot]=timer->next;
            alloutwheel[timer->time_wheelID][timer->time_slot]->prev=NULL;
            return timer;
           }
    }
    else if (timer->next==NULL)
    {
        timer->prev->next=NULL;
        return timer;
    }
    else
        timer->prev->next=timer->next;
        timer->next->prev=timer->prev;
        return timer;
    }

//删除定时器
    void delete_timerlist(tw_timer* timer)
    {
        tw_timer* tmp=timer;
        if(timer->prev!=NULL)
        {
            std::cout<<"tick call is wrong"<<std::endl;;
            return;
        }
        else
        {   while (timer!=NULL)
            {
                timer=tmp;
                timer->cb_func(timer->user_data);
                tmp=timer->next;
                delete timer;
            }
        
        }
        
    }
//移动定时器
    void move_timerlist(tw_timer* tmp, int wheelid)
    {
        tw_timer* s=tmp;
        if(wheelid==1)
        {
            while(tmp!=NULL)
            {
                inwheel_add(tmp->all_wheel_slot[0],tmp);
            }
            alloutwheel[s->time_wheelID][s->time_slot]=NULL;
        }
        else if(1<wheelid<5)
        {
            while(tmp!=NULL)
            {
                outwheel_add(wheelid,tmp->all_wheel_slot[wheelid-1],tmp);
            }
            alloutwheel[s->time_wheelID][s->time_slot]=NULL;
        }
        else
        {
            std::cout<<"move_timer wrong."<<std::endl;
        }
    }
};
#endif
