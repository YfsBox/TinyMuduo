//
// Created by 杨丰硕 on 2023/1/9.
//

#ifndef TINYMUDUO_ACCEPTOR_H
#define TINYMUDUO_ACCEPTOR_H

#include <atomic>
#include <functional>
#include "Socket.h"
#include "../base/Utils.h"
#include "../reactor/Channel.h"

namespace TinyMuduo {
    class EventLoop;
    class SockAddress;
    class Acceptor: Utils::noncopyable {
    public:
        using NewConnectionCb = std::function<void(int, SockAddress &)>;      // 分别表示返回的描述符和地址

        Acceptor(EventLoop *loop, const SockAddress &addr);

        ~Acceptor();

        bool isListening() const {
            return is_listen_;          // 怎样才算是listen呢?应该说是加入到其Eve
            // ntloop之后,并且注册到了其中的epoller中
        }

        void listen();

        void setNewConnectionCb(NewConnectionCb cb) {
            newconnection_func_ = std::move(cb);
        }

        int getListeningFd() const {
            return socket_.getFd();
        }

    private:
        void handleNewConn();

        EventLoop* owner_loop_;
        std::atomic_bool is_listen_;
        Socket socket_;
        Channel channel_;
        NewConnectionCb newconnection_func_;
        int idel_fd_;
    };
}

#endif //TINYMUDUO_ACCEPTOR_H
