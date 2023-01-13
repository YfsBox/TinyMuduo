//
// Created by 杨丰硕 on 2023/1/9.
//
#include <string.h>
#include "Acceptor.h"
#include "TcpServer.h"
#include "../logger/Logger.h"
#include "../base/Threadpool.h"

using namespace TinyMuduo;

TcpServer::TcpServer(const std::string &name, const std::string ip, uint32_t port,
                     EventLoop *loop, size_t poolsize,
                     size_t queuesize, const std::string pool_name):
                     is_running_(false),ip_(ip),
                     port_(port),name_(name),
                     address_(ip, port),loop_(loop),
                     acceptor_(std::make_unique<Acceptor>(loop_, address_)),
                     pool_(std::make_unique<ThreadPool>(poolsize, queuesize, pool_name)),
                     curr_conn_number_(0){
    acceptor_->setNewConnectionCb(std::bind(&TcpServer::newConnFunc, this, std::placeholders::_1,
                                            std::placeholders::_2));

}


TcpServer::~TcpServer() {
    // 将连接关闭

}

void TcpServer::start() {
    loop_->assertInThisThread();
    if (is_running_) {
        return;
    }
    is_running_ = true;
    acceptor_->listen();    // 开启监听
    pool_->start();
}

void TcpServer::stop() {
    loop_->assertInThisThread();
    if (!is_running_) {
        return;
    }
    pool_->stop();
    is_running_ = false;
}

void TcpServer::newConnFunc(int fd, SockAddress &address) {
    // 根据fd和address创建一个Connection
    char buf[64];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "#%s:%uz:%uz", ip_.c_str(), port_, curr_conn_number_++);
    LOG_INFO << "A new connection come(" << buf << ")";
    // 将该TcpConnection加入到map

    // 并且设置callback注册到epoller等等


    // 创建出来这个之后,还需要执行established相关的回调,这部分回调将会从线程池中找出来一个io线程进行处理

}

