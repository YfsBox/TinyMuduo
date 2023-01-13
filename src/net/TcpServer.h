//
// Created by 杨丰硕 on 2023/1/9.
//

#ifndef TINYMUDUO_TCPSERVER_H
#define TINYMUDUO_TCPSERVER_H

#include <memory>
#include <map>
#include <functional>
#include "../base/Utils.h"
#include "../net/SockAddress.h"

namespace TinyMuduo {

    class EventLoop;

    class ThreadPool;

    class Acceptor;

    class TcpConnection;

    class TcpServer: public Utils::noncopyable {
    public:
        // using NewConnectionFunc = std::function<void(int, SockAddress &)>;
        using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
        using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

        TcpServer(const std::string &name, const std::string ip, uint32_t port,
                  EventLoop *loop, size_t poolsize, size_t queuesize,
                  const std::string pool_name);

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

    private:

        void newConnFunc(int fd, SockAddress &address);

        bool is_running_;

        std::string ip_;        // 服务器监听的ip以及端口
        uint32_t port_;

        std::string name_;
        const SockAddress address_;     // 表示的是所监听点的address

        EventLoop *loop_;        // Epoller的生命其实不为Server所控制

        std::unique_ptr<Acceptor> acceptor_;        // 其中Accpetor
        std::unique_ptr<ThreadPool> pool_;

        uint32_t curr_conn_number_;     // 用来给connection分配name用的标号
        ConnectionMap connection_map_;

    };
}

#endif //TINYMUDUO_TCPSERVER_H
