#include "socketqueue.h"


Socketqueue::Socketqueue(int size)
    :set_size(size)
{

}

Socketqueue::~Socketqueue()
{

}

int Socketqueue::initsocketqueue()
{
    unique_lock<boost::mutex> lock(m_mutex);
    if(this->is_empty())
        return 0;
    else{
        while(!this->is_empty())
        {
            m_queue.pop();
        }
        return 0;
    }
    return 1;
}

int Socketqueue::push(Socket* socket)
{
    if(socket == nullptr)
        return 1;
    unique_lock<boost::mutex> lock(m_mutex);
    while(this->is_full())
    {
        //cout<<"full waiting..."<<endl;
        cond_push.wait(lock);
        //cout<<"full waited..."<<endl;
    }
    m_queue.push(socket);
    ++curr_num;
    cond_pop.notify_one();
    return 0;
}

Socket* Socketqueue::pop()
{
    unique_lock<boost::mutex> lock(m_mutex);
    while(this->is_empty())
    {
        //cout<<"empty waiting..."<<endl;
        cond_pop.wait(lock);
        //cout<<"empty waited..."<<endl;
    }
    boost::asio::ip::tcp::socket* p=nullptr;
    p = m_queue.front();
    m_queue.pop();
    --curr_num;
    cond_push.notify_one();
    return p;
}

bool Socketqueue::is_full()
{
    return (this->curr_num==set_size);
}

bool Socketqueue::is_empty()
{
    return (this->curr_num==0);
}

int Socketqueue::size()
{
    return (this->curr_num);
}

