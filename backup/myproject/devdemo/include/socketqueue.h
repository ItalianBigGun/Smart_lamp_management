#ifndef _SOCKETQUEUE_H_
#define _SOCKETQUEUE_H_

#include "socketqueue.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <condition_variable>
#include <string> 
#include <iostream>
#include <queue>

using namespace std;

using Socket = boost::asio::ip::tcp::socket;
using SocketPtr = boost::shared_ptr<Socket>;

struct socketnode
{
    Socket *socket;
    int status = 0;
};
class Socketqueue
{
    public:
        Socketqueue(int size);
        ~Socketqueue();

        int initsocketqueue();       //
        bool is_full();              //
        bool is_empty();             //
        int size();

        int push(Socket* socket);
        Socket* pop();
    private:
        int set_size=0;
        int curr_num = 0;
        std::queue<Socket*> m_queue;
        //SocketPtr m_socket(new Socket());
        boost::mutex m_mutex;
        std::condition_variable_any cond_push,cond_pop;
        void *buffer = nullptr;
};
















#endif
