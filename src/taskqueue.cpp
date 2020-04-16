#include "taskqueue.h"

Taskqueue::Taskqueue(int size)
{
    set_size = size;
}

Taskqueue::~Taskqueue()
{
    std::cout<<"taskqueue end"<<std::endl;
    //clear();
}

int Taskqueue::inittaskqueue()
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

int Taskqueue::push(struct tasknode* task)
{
    unique_lock<boost::mutex> lock(m_mutex);
    while(this->is_full())
    {
        cout<<"taskqueue full waiting..."<<endl;
        cond_push.wait(lock);
        cout<<"taskqueue full waited..."<<endl;
        if(!use_flag){
            return 1;
        }
    }
    m_queue.push(task);
    ++curr_num;
    cond_pop.notify_one();
    return 0;
}

struct tasknode* Taskqueue::pop()
{
    unique_lock<boost::mutex> lock(m_mutex);
    while(this->is_empty())
    {
        cout<<"taskqueue empty waiting..."<<endl;
        cond_pop.wait(lock);
        cout<<"taskqueue empty waited..."<<endl;
        if(!use_flag){
            return nullptr;
        }
    }
    struct tasknode* p=nullptr;
    p = m_queue.front();
    m_queue.pop();
    --curr_num;
    cond_push.notify_one();
    return p;
}

bool Taskqueue::is_full()
{
    return (this->curr_num==set_size);
}

bool Taskqueue::is_empty()
{
    return (this->curr_num==0);
}

int Taskqueue::size()
{
    return (this->curr_num);
}

void Taskqueue::clear()
{
    std::queue<struct tasknode*> empty;
    swap(empty, m_queue);
}

void Taskqueue::distroy()
{
    use_flag = false;
    printf("%d\n",__LINE__);
    //this->m_mutex.unlock();
    printf("%d\n",__LINE__);
    this->cond_push.notify_all();
    printf("%d\n",__LINE__);
    this->cond_pop.notify_all();
    printf("%d\n",__LINE__);
}
