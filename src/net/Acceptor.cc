//
// Created by 杨丰硕 on 2023/1/9.
//
#include <unistd.h>
#include "Acceptor.h"
#include "SockAddress.h"

using namespace TinyMuduo;

Acceptor::Acceptor(EventLoop *loop, NewConnectionCb cb, const SockAddress &addr):
    owner_loop_(loop),
    is_listen_(false),
    socket_(),
    channel_(loop, socket_.getFd()),
    newconnection_func_(std::move(cb)){
    // 需要结合socket以及channel进行调整
    socket_.bindSockAddress(addr);  // 还需要设置为非阻塞I/O
    // 将socket设置为非阻塞I/O
    channel_.setReadable();         // 除了修改event，还应该对epoller进行修改，而对epoller修改需要间接地通过loop修改
    channel_.setReadCallBack(std::bind(&Acceptor::handleNewConn, this));
}

void Acceptor::listen() {
    is_listen_ = true;
    socket_.listen();
}

void Acceptor::handleNewConn() {
    // 可以对该socket调用accept
    SockAddress peer_addr;
    auto accept_fd = socket_.accept(&peer_addr);
    if (accept_fd < 0) {
        // 错误的情况
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
