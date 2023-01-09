//
// Created by 杨丰硕 on 2023/1/8.
//
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include "SockAddress.h"

using namespace TinyMuduo;

SockAddress::SockAddress() {
    memset(&sockaddr_in_, 0, sizeof(sockaddr_in_)); // 直接清空初始化
}

SockAddress::SockAddress(uint32_t port) {       // 这个时候其相关IP就是any，也就是0.0.0.0
    memset(&sockaddr_in_, 0, sizeof(sockaddr_in_));
    sockaddr_in_.sin_family = AF_INET;
    sockaddr_in_.sin_addr.s_addr = INADDR_ANY;
    sockaddr_in_.sin_port = htons(port);
}

SockAddress::SockAddress(const sockaddr_in &addr): sockaddr_in_(addr) {}

SockAddress::SockAddress(const std::string &ip, uint32_t port) {
    memset(&sockaddr_in_, 0, sizeof(sockaddr_in_));
    sockaddr_in_.sin_family = AF_INET;
    if(::inet_pton(AF_INET, ip.c_str(), &sockaddr_in_.sin_addr.s_addr) != 1){
        assert(1); // 出错的情况
    }
    sockaddr_in_.sin_port = htons(port);
}

std::string SockAddress::getIp() const {
    char buf[64] = {0};
    if(::inet_ntop(AF_INET, &sockaddr_in_.sin_addr,  buf, sizeof(buf)) == nullptr) {
        assert(1);
    }
    return buf;
}

uint16_t SockAddress::getPort() const {
    return ::ntohs(sockaddr_in_.sin_port);
}

void SockAddress::setSockaddr(const sockaddr_in &sockaddrIn) {
    sockaddr_in_ = sockaddrIn;
}





