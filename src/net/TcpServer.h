//
// Created by 杨丰硕 on 2023/1/9.
//

#ifndef TINYMUDUO_TCPSERVER_H
#define TINYMUDUO_TCPSERVER_H

#include <memory>
#include <functional>

namespace TinyMuduo {

    class EventLoop;

    class ThreadPool;

    class Acceptor;

    class SockAddress;

    class TcpServer {
    public:
        using NewConnectionFunc = std::function<void(int, SockAddress &)>;


    private:

        std::string ip_addr_;
        uint16_t port_;

        EventLoop *loop;        // Epoller的生命其实不为Server所控制

        std::unique_ptr<Acceptor> acceptor_;
        std::unique_ptr<ThreadPool> pool_;
        NewConnectionFunc new_connection_func_;
    };
}

#endif //TINYMUDUO_TCPSERVER_H
