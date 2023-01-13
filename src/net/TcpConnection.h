//
// Created by 杨丰硕 on 2023/1/9.
//

#ifndef TINYMUDUO_TCPCONNECTION_H
#define TINYMUDUO_TCPCONNECTION_H

#include <atomic>
#include <functional>
#include <memory>
#include "../base/Buffer.h"
#include "../net/SockAddress.h"

namespace TinyMuduo {
    class Socket;
    class Channel;
    class EventLoop;
    class TcpConnection {
    public:
        using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
        enum ConnectionState {
            Connecting = 0,
            Connected,
        };

        TcpConnection(EventLoop *loop,
                      const std::string &name,
                      int fd,
                      const SockAddress &local,
                      const SockAddress &peer);

        ~TcpConnection();


    private:
        void readHandle();
        void writeHandle();
        void errorHandle();
        void closeHandle();

        EventLoop *loop_;

        std::atomic_int state_;
        std::string conn_name_;

        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;

        const SockAddress local_addr_;
        const SockAddress peer_addr_;

        Buffer read_buffer_;
        Buffer write_buffer_;
    };
}

#endif //TINYMUDUO_TCPCONNECTION_H
