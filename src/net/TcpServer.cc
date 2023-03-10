//
// Created by 杨丰硕 on 2023/1/9.
//
#include <cstring>
#include "Acceptor.h"
#include "TcpServer.h"
#include "../net/TcpConnection.h"
#include "../logger/Logger.h"
#include "../reactor/LoopThreadPool.h"

using namespace TinyMuduo;

TcpServer::TcpServer(const std::string &name, const std::string ip, uint32_t port,
                     size_t io_threads_num, EventLoop *loop,const std::string &pool_name):
                     is_running_(false),
                     ip_(ip),
                     port_(port),
                     io_threads_num_(io_threads_num),
                     name_(name),
                     address_(ip, port),
                     loop_(loop),
                     acceptor_(std::make_unique<Acceptor>(loop_, address_)),
                     pool_(std::make_unique<LoopThreadPool>(loop_, pool_name)),
                     curr_conn_number_(0){
    acceptor_->setNewConnectionCb(std::bind(&TcpServer::newConnFunc, this, std::placeholders::_1,
                                            std::placeholders::_2));

}

TcpServer::~TcpServer() {
    // 将连接关闭
    for (auto &[name, conn] : connection_map_) {
        TcpConnectionPtr connptr(conn);
        conn.reset();
        connptr->getLoop()->runInLoop(std::bind(&TcpConnection::destroy, connptr));
    }
}

void TcpServer::start() {
    loop_->assertInThisThread();
    if (is_running_) {
        return;
    }
    is_running_ = true;
    loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
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
    // 首先获取一个io loop用来处理
    auto io_loop = pool_->getNextLoop();
    // 根据fd和address创建一个Connection
    char name_buf[ip_.length() + 32];
    memset(name_buf, 0, sizeof(name_buf));
    sprintf(name_buf, "#%s:%u:%u", ip_.c_str(), port_, curr_conn_number_++);
    std::string conn_name = name_buf;
    // 获取local addr
    sockaddr_in localaddr;
    bzero(&localaddr, sizeof(localaddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(localaddr));
    if (::getsockname(acceptor_->getListeningFd(),
                      reinterpret_cast<sockaddr*>(&localaddr), &addrlen) < 0) {
        LOG_ERROR << "Get local sockname error in TcpServer, the acceptor fd is " << acceptor_->getListeningFd();
    }
    SockAddress local_address(localaddr);
    // 创建一个TcpConnection连接
    auto conn = std::make_shared<TcpConnection>(io_loop, conn_name, fd,
                                                local_address, address);
    connection_map_[conn->getName()] = conn;
    // LOG_INFO << "A new connection come(" << name_buf << ")" << " the use cnt is " << conn.use_count();
    // 创建出来这个之后,还需要执行established相关的回调,这部分回调将会从线程池中找出来一个io线程进行处理
    conn->setCloseCallback(std::bind(&TcpServer::removeConnForClose, this, std::placeholders::_1));          // 将会在closeHandle中被调用
    // conn->setCloseCallback(std::bind(&TcpServer::removeConnForClose, this, conn))    这里因为有个引用技术减不到1而导致资源释放不了的
    conn->setConnectionCallback(connection_callback_);
    conn->setMessageCallback(message_callback_);         // 设置回调函数

    io_loop->runInLoop(std::bind(&TcpConnection::establish, conn));

}
// 提供给close callback，也就是TcpConnection中的closeHandle
void TcpServer::removeConnForClose(const TcpConnectionPtr &conn) {
    loop_->runInLoop(std::bind(&TcpServer::removeConnForCloseInLoop, this, conn));
}

void TcpServer::removeConnForCloseInLoop(const TcpConnectionPtr &conn) {
    auto tmp_fd = conn->getFd();
    // LOG_DEBUG << "the use count is " << conn.use_count() << " at begin in removecb";
    auto findit = connection_map_.find(conn->getName());
    if (findit != connection_map_.end()) {
        connection_map_.erase(findit);
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::destroy,
                                             conn));
    }
    /*LOG_DEBUG << "the use count is " << conn.use_count() << " the fd is "
    << tmp_fd;*/
}

// 如果client端关闭连接,server端就会触发closeHandle,
// 如果这个时候还会有send的话,

