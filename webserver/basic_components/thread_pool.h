#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "threadsafe_data_structure/threadsafe_queue.h"

template< typename T >
class threadpool
{
public:
    threadpool( int thread_number = 8, int max_requests = 10000 );
    ~threadpool();
    void append( T* request );

private:
    static void* worker( void* arg );
    void run();

private:
    int m_thread_number;
    int m_max_requests;
    pthread_t* m_threads;
    threadsafe_queue<T*> tsqueue;
    bool m_stop;
};

//创建线程池
template< typename T >
threadpool< T >::threadpool( int thread_number, int max_requests ) : 
        m_thread_number( thread_number ), m_max_requests( max_requests ), m_stop( false ), m_threads( NULL )
{
    if( ( thread_number <= 0 ) || ( max_requests <= 0 ) )
    {
        throw std::exception();
    }

    m_threads = new pthread_t[ m_thread_number ];
    if( ! m_threads )
    {
        throw std::exception();
    }

    for ( int i = 0; i < thread_number; ++i )
    {
        printf( "create the %dth thread\n", i );
        if( pthread_create( m_threads + i, NULL, worker, this ) != 0 )
        {
            delete [] m_threads;
            throw std::exception();
        }
        if( pthread_detach( m_threads[i] ) )
        {
            delete [] m_threads;
            throw std::exception();
        }
    }
}

template< typename T >
threadpool< T >::~threadpool()
{
    delete [] m_threads;
    m_stop = true;
}
//主线程调用接口
template< typename T >
void threadpool< T >::append( T* request )
{
    tsqueue.push(request);
}

template< typename T >
void* threadpool< T >::worker( void* arg )
{
    threadpool* pool = ( threadpool* )arg;
    pool->run();
    return pool;
}

template< typename T >
void threadpool< T >::run()
{
    while ( ! m_stop )
    {   
        T* request=nullptr;
        tsqueue.wait_and_pop(request);
        request->process();
    }
}

#endif