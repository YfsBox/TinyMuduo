//
// Created by 杨丰硕 on 2023/1/16.
//
#include <any>
#include "HttpServer.h"
#include "HttpParser.h"
#include "HttpReponse.h"
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
    LOG_INFO << "the conn fd is " << conn->getFd() << " in onMessage";
    HttpParser* parser = std::any_cast<HttpParser>(conn->getContext());         // 此时已经

    if (!parser->parsing(buffer)) {     // 中间出现了错误
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (parser->isFinishAll()) {
        // LOG_DEBUG << "the request is finished looking";
        onRequest(conn, parser->getRequest());
        parser->reset();     // 同时清空
    }

}

void HttpServer::onConn(const TinyMuduo::TcpConnection::TcpConnectionPtr &conn) {
    if (conn->isConnected()) {      // 通过状态Connected还是DisConnected来区分Conn回调中的行为
        LOG_INFO << "the Conn fd " << conn->getFd() << " set the Context";
        conn->setContext(HttpParser());
    }
}

void HttpServer::onRequest(const TinyMuduo::TcpConnection::TcpConnectionPtr &conn, const HttpRequest &request) {
  // 首先提取出来close,判断是否需要close
    const std::string& connection = request.lookup("Connection");
    bool close = (connection == "close") ||
            (request.getVersion() == HttpRequest::VHttp10 && connection != "Keep-Alive");
    // 创建出response
    TinyHttp::HttpReponse response(close);
    http_callback_(request, &response);      // 根据生成的request设置response的内容
    Buffer buf;
    response.appendToBuffer(&buf);    // 加入到buffer中并且send
    conn->send(std::string(buf.peek(), buf.getReadableSize()));

    if (response.getCloseConnection()) {    // 判断服务端是否需要将连接关闭
        conn->shutdown();
    }

}

