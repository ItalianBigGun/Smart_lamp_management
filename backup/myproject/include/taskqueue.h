#ifndef _TASKQUEUE_H_
#define _TASKQUEUE_H_

#include "taskqueue.h"
#include "test.pb.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <condition_variable>
#include <string> 
#include <iostream>
#include <queue>

using namespace std;

using Socket = boost::asio::ip::tcp::socket;

struct tasknode
{
    Socket *socket;
    my::Node taskmsg;
};

class Taskqueue
{
    public:
        Taskqueue(int size);
        ~Taskqueue();

        int inittaskqueue();       //
        bool is_full();              //
        bool is_empty();             //
        int size();
        void clear();

        static int login(Socket *socket);

        int push(struct tasknode* task);
        struct tasknode* pop();
    private:

        int set_size=0;
        int curr_num = 0;
        std::queue<struct tasknode*> m_queue;
        //SocketPtr m_socket(new Socket());
        boost::mutex m_mutex;
        std::condition_variable_any cond_push,cond_pop;
        void *buffer = nullptr;
};















#endif