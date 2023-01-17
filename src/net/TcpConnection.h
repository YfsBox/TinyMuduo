//
// Created by 杨丰硕 on 2023/1/9.
//

#ifndef TINYMUDUO_TCPCONNECTION_H
#define TINYMUDUO_TCPCONNECTION_H

#include <any>
#include <atomic>
#include <functional>
#include <memory>
#include "../net/Socket.h"
#include "../base/Buffer.h"
#include "../net/SockAddress.h"

namespace TinyMuduo {
    class Socket;
    class Channel;
    class EventLoop;
    class TcpConnection: public std::enable_shared_from_this<TcpConnection> {
        // enable_shared_from_this的作用在于可以从class内部获得一个this的shared_ptr,而不是搞出来独立的shared_ptr
    public:
        using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
        using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
        using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer*, TimeStamp)>;
        using CloseCallback = std::function<void(const TcpConnectionPtr&)>;

        enum ConnectionState {
            Connecting = 0,
            Connected,
            DisConnecting,
            DisConnected,
        };

        TcpConnection(EventLoop *loop,
                      const std::string &name,
                      int fd,
                      const SockAddress &local,
                      const SockAddress &peer);

        ~TcpConnection();

        void setConnectionCallback(ConnectionCallback cb) {
            connection_callback_ = std::move(cb);
        }

        void setMessageCallback(MessageCallback cb) {
            message_callback_ = std::move(cb);
        }

        void setCloseCallback(CloseCallback cb) {
            close_callback_ = std::move(cb);
        }

        void setState(ConnectionState state) {
            state_ = state;
        }

        EventLoop *getLoop() const {
            return loop_;
        }

        std::string getName() const {
            return conn_name_;
        }

        SockAddress getLocalAddr() const {
            return local_addr_;
        }

        SockAddress getPeerAddr() const {
            return peer_addr_;
        }

        bool isConnected() const {
            return state_ == ConnectionState::Connected;
        }

        void establish();

        void destroy();

        void send(const std::string &content);

        void send(const char *content, size_t len);

        void shutdown();

        const std::any &getConstContext() const {
            return context_;
        }

        std::any *getContext() {
            return &context_;
        }

        void setContext(const std::any &context) {
            context_ = context;
        }

        int getFd() const {
            return socket_->getFd();
        }

    private:
        void readHandle();
        void writeHandle();
        void errorHandle();
        void closeHandle();

        void sendInLoop(const char *content, size_t len);

        void shutdownInLoop();

        EventLoop *loop_;

        std::atomic_int state_;
        std::string conn_name_;

        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;

        const SockAddress local_addr_;
        const SockAddress peer_addr_;

        Buffer read_buffer_;
        Buffer write_buffer_;

        ConnectionCallback connection_callback_;
        MessageCallback message_callback_;
        CloseCallback close_callback_;

        std::any context_;          // 可以用来维护TcpConnection中的状态变量
    };
}

#endif //TINYMUDUO_TCPCONNECTION_H

// 还需要了解的是connection会有那些相应的事件,尤其是读、写的过程中又会有那些细节需要注意。
