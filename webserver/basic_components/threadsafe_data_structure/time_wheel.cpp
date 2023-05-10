#include "time_wheel.h"
//添加定时器
tw_timer* time_wheel::add_timer(int timeout)
{
   
    int slot=0;
    tw_timer timer(0);
    int wheelID=locate_wheel_and_update_slots(timeout,&timer);
   
    switch (wheelID)
    {
    case 1:
        slot=timeout;
        return inwheel_add(slot,&timer);
        break;
    case 2:
        slot=timeout/inwheel;
        return outwheel_add(2,slot,&timer);
        break;
    case 3:
        slot=timeout/(inwheel*outwheel);
        return outwheel_add(3,slot,&timer);
        break;
    case 4:
        slot=timeout/(inwheel*outwheel*outwheel);
        return outwheel_add(4,slot,&timer);
        break;
    case 5:
        slot=timeout/(inwheel*outwheel*outwheel*outwheel);
        return outwheel_add(5,slot,&timer);
        break;
    default:
        printf("timeout is wrong.");
        break;
    }

}

//删除定时器
void time_wheel::del_timer(tw_timer* timer)
{
    delete del_timer_from_wheellist(timer);
}

//更新定时器
tw_timer* time_wheel::update_timer(tw_timer* timer,int timeout)
{
    tw_timer* tmp=del_timer_from_wheellist(timer);
    delete tmp;
    return add_timer(timeout); 
}

//一次滴答
void time_wheel::tick()
{   
    delete_timerlist(inwheel_slot[cur_slot[0]]);
    if(cur_slot[0]<inwheel)
    {
        ++cur_slot[0];
    }
    else if(cur_slot[0]==inwheel)
    {
        move_timerlist(alloutwheel[1][1],1);
        cur_slot[0]=1;
        for(int i=2;i<5;i++)
        {
            if(cur_slot[i-1]<outwheel)
            {
                ++cur_slot[i-1];
                return;
            }
            else if (cur_slot[i-1]==outwheel)
            {
                move_timerlist(alloutwheel[i][1],i);
                cur_slot[i-1]=1;
            }
            
        }
    }
}

