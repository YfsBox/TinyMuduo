//
// Created by 杨丰硕 on 2023/1/8.
//
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include "Socket.h"
#include "SockAddress.h"
#include "../logger/Logger.h"

using namespace TinyMuduo;

Socket::Socket(int fd):fd_(fd) {}

Socket::Socket():fd_(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                              IPPROTO_TCP)) {       // 创建了socket
    if (fd_ < 0) {
        printf("Create Socket error\n");
        assert(1);
    }
}

Socket::~Socket() {
    // LOG_DEBUG << "close fd " << fd_;
    ::close(fd_);
}

int Socket::bindSockAddress(const SockAddress &socketAddress) {
    int ret = ::bind(fd_, socketAddress.getSockAddr(), socketAddress.getSocketLen());
    if (ret < 0) {
        printf("bind addr to socket %d error\n", fd_);
    }
    return ret;
}

int Socket::accept(SockAddress *sockAddress) {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    int connfd = ::accept4(fd_, reinterpret_cast<sockaddr*>(&addr),
                           &len, SOCK_NONBLOCK | SOCK_CLOEXEC);  // 非阻塞,CLOEXC不太懂
    if(connfd >= 0){
        sockAddress->setSockaddr(addr);
    } else {
        // 这种情况是accept出错的情况
        LOG_FATAL << "the accept error, error no is " << errno;
    }
    return connfd;
}

int Socket::listen() {
    int ret = ::listen(fd_, 1024);
    if (ret < 0) {
        LOG_FATAL << "the listen error, error no is " << errno;
    }
    return ret;
}

int Socket::connect(const SockAddress *sockAddress) {
    int ret = ::connect(fd_, sockAddress->getSockAddr(), sockAddress->getSocketLen());
    if (ret < 0) {
        LOG_FATAL << "the connect error, error no is " << errno;
    }
    return ret;
}

void Socket::setNoBlocking() {

}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::setReusePort(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::shutdownWrite(int fd) {
    ssize_t ret = ::shutdown(fd, SHUT_WR);
    if (ret < 0) {
        LOG_ERROR << "shut down the fd error " << fd;
    }
}

