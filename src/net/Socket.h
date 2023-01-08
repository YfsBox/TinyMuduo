//
// Created by 杨丰硕 on 2023/1/8.
//

#ifndef TINYMUDUO_SOCKET_H
#define TINYMUDUO_SOCKET_H

namespace TinyMuduo {

    class SocketAddress;
    class Socket {
    public:

        Socket();       // 其中会有对socket的创建

        ~Socket();

        int getFd() const {
            return fd_;
        }

        int bindSockAddress(const SocketAddress& socketAddress);    // 将socket和某个具体的地址绑定

        int listen();   // 监听某个socket

        int accept();   // 接收上面到来的连接

        int connect(); // 这个将会用于client

    private:
        const int fd_;
    };

}

#endif //TINYMUDUO_SOCKET_H
