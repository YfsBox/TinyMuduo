//
// Created by 杨丰硕 on 2023/1/8.
//
#ifndef TINYMUDUO_SOCKADDRESS_H
#define TINYMUDUO_SOCKADDRESS_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

namespace TinyMuduo {

    class SockAddress {
    public:

        SockAddress();

        explicit SockAddress(uint32_t port);

        SockAddress(const std::string &ip, uint32_t port);

        SockAddress(const sockaddr_in &addr);   // 这个允许其进行隐式构造

        ~SockAddress() = default;

        std::string getIp() const;

        uint16_t getPort() const;

        const sockaddr_in getSockAddrIn() const {
            return sockaddr_in_;
        }

        const sockaddr *getSockAddr() const {
            return reinterpret_cast<const sockaddr*>(&sockaddr_in_);
        }

        socklen_t getSocketLen() const {
            return sizeof(*getSockAddr());
        }

        void setSockaddr(const sockaddr_in &sockAddress);

    private:

        sockaddr_in sockaddr_in_;

    };
}

#endif //TINYMUDUO_SOCKADDRESS_H
