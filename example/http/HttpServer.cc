//
// Created by 杨丰硕 on 2023/1/16.
//
#include "HttpServer.h"
#include "../../src/base/Timestamp.h"
#include "../../src/base/Buffer.h"

using namespace TinyHttp;
using namespace TinyMuduo;

HttpServer::HttpServer(const std::string &name,
                       const std::string &ip,
                       uint32_t port, size_t io_threads_num,
                       TinyMuduo::EventLoop *loop, const std::string pool_name):
        server_(name, ip, port, io_threads_num, loop, pool_name){
    server_.setIoThreadNumber(io_threads_num);
    server_.setConnCb(std::bind(&HttpServer::onConn, this, std::placeholders::_1));
    server_.setMessageCb(std::bind(&HttpServer::onMessage, this, std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3));
    // 注册回调
}

void HttpServer::start() {
    LOG_DEBUG << "Server " << server_.getName()
    << "start and listening on port " << server_.getPort();
    server_.start();
}

void HttpServer::onMessage(const TcpServer::TcpConnectionPtr &conn, Buffer *buffer,
                           TimeStamp timestamp) {

}

void HttpServer::onConn(const TinyMuduo::TcpConnection::TcpConnectionPtr &conn) {

}

