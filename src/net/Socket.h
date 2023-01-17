//
// Created by 杨丰硕 on 2023/1/8.
//

#ifndef TINYMUDUO_SOCKET_H
#define TINYMUDUO_SOCKET_H

namespace TinyMuduo {
    class SockAddress;
    class Socket {
    public:

        explicit Socket(int fd);

        Socket();       // 其中会有对socket的创建

        ~Socket();

        Socket(const Socket&) = default;

        Socket& operator=(const Socket&) = default;

        int getFd() const {
            return fd_;
        }

        int bindSockAddress(const SockAddress& socketAddress);    // 将socket和某个具体的地址绑定

        int listen();   // 监听某个socket

        int accept(SockAddress *sockAddress);   // 接收上面到来的连接

        int connect(const SockAddress *sockAddress); // 这个将会用于client

        void setNoBlocking();

        void setReuseAddr(bool on);

        void setReusePort(bool on);

        static void shutdownWrite(int fd);

    private:
        const int fd_;
    };

}

#endif //TINYMUDUO_SOCKET_H
