//
// Created by 杨丰硕 on 2023/1/9.
//

#ifndef TINYMUDUO_TCPSERVER_H
#define TINYMUDUO_TCPSERVER_H

#include <memory>
#include <map>
#include <functional>
#include "../base/Utils.h"
#include "../reactor/LoopThreadPool.h"
#include "../net/SockAddress.h"

namespace TinyMuduo {

    class Buffer;

    class EventLoop;

    class LoopThreadPool;

    class Acceptor;

    class TcpConnection;

    class TcpServer: public Utils::noncopyable {
    public:
        // using NewConnectionFunc = std::function<void(int, SockAddress &)>;
        using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
        using ConnectionMap = std::map<std::string, TcpConnectionPtr>;
        using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
        using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer*, TimeStamp)>;
        using CloseCallback = std::function<void(const TcpConnectionPtr&)>;

        TcpServer(const std::string &name, const std::string ip, uint32_t port,
                  size_t io_threads_num, EventLoop *loop,const std::string &pool_name);

        ~TcpServer();

        void start();

        void stop();

        const SockAddress getAddr() const {
            return address_;
        }

        std::string getName() const {
            return name_;
        }

        bool isRunning() const {
            return is_running_;
        }

        const EventLoop* getLoop() const {
            return loop_;
        }

        std::string getIp() const {
            return ip_;
        }

        uint32_t getPort() const {
            return port_;
        }

        void setIoThreadNumber(size_t num) {
            io_threads_num_ = num;
            pool_->setThreadSize(io_threads_num_);
        }

        void setConnCb(ConnectionCallback cb) {
            connection_callback_ = cb;
        }

        void setMessageCb(MessageCallback cb) {
            message_callback_ = cb;
        }

    private:

        void newConnFunc(int fd, SockAddress &address);

        void removeConnForClose(const TcpConnectionPtr& conn);

        void removeConnForCloseInLoop(const TcpConnectionPtr &conn);

        bool is_running_;

        std::string ip_;        // 服务器监听的ip以及端口
        uint32_t port_;
        size_t io_threads_num_;

        std::string name_;
        const SockAddress address_;     // 表示的是所监听点的address

        EventLoop *loop_;        // Epoller的生命其实不为Server所控制

        std::unique_ptr<Acceptor> acceptor_;        // 其中Accpetor
        std::unique_ptr<LoopThreadPool> pool_;

        uint32_t curr_conn_number_;     // 用来给connection分配name用的标号
        ConnectionMap connection_map_;
        // 下面还应该包含一些TcpConnection需要进行set的函数
        ConnectionCallback connection_callback_;
        MessageCallback message_callback_;
    };
}

#endif //TINYMUDUO_TCPSERVER_H
