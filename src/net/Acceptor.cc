//
// Created by 杨丰硕 on 2023/1/9.
//
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "Acceptor.h"
#include "SockAddress.h"

using namespace TinyMuduo;

Acceptor::Acceptor(EventLoop *loop, const SockAddress &addr):
    owner_loop_(loop),
    is_listen_(false),
    socket_(),
    channel_(loop, socket_.getFd()),
    idel_fd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)){
    // 需要结合socket以及channel进行调整
    socket_.setReusePort(true);
    socket_.setReuseAddr(true);
    socket_.bindSockAddress(addr);  // 还需要设置为非阻塞I/O
    channel_.setReadCallBack(std::bind(&Acceptor::handleNewConn, this));
}

Acceptor::~Acceptor() {
    channel_.setEvent(Channel::NULL_EVENT);         // 相当于调用了disablall
}

void Acceptor::listen() {
    is_listen_ = true;
    socket_.listen();
    channel_.setReadable();         // 除了修改event，还应该对epoller进行修改，而对epoller修改需要间接地通过loop修改
}

void Acceptor::handleNewConn() {
    // 可以对该socket调用accept
    SockAddress peer_addr;
    auto accept_fd = socket_.accept(&peer_addr);
    if (accept_fd < 0) {
        // 错误的情况
        if (errno == EMFILE) {      // 错误类型对应了文件描述符耗尽的类型
            ::close(idel_fd_);
            ::accept(idel_fd_, nullptr, nullptr);
            ::close(idel_fd_);
            idel_fd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    } else {    // 表示正常返回,执行
        if (newconnection_func_) {
            newconnection_func_(accept_fd, peer_addr);         // 执行回调函数
        } else {
            ::close(accept_fd);         // 关闭返回的socket
        }
        // 加入到该循环对应的epoll中
    }
}

// 为什么reactor需要结合非阻塞IO工作????
