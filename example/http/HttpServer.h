//
// Created by 杨丰硕 on 2023/1/16.
//

#ifndef TINYMUDUO_HTTPSERVER_H
#define TINYMUDUO_HTTPSERVER_H

#include <memory>
#include "../../src/base/Timestamp.h"
#include "../../src/net/TcpServer.h"
#include "../../src/net/TcpConnection.h"

namespace TinyMuduo {
    class EventLoop;
    class Buffer;
}

namespace TinyHttp {
    class HttpRequest;

    class HttpReponse;

    class HttpServer {
    public:
        using HttpCallback = std::function<void (const HttpRequest&, HttpReponse*)>;
        
        HttpServer(const std::string &name, const std::string &ip, uint32_t port,
                   size_t io_threads_num, TinyMuduo::EventLoop *loop, const std::string pool_name);

        ~HttpServer() = default;

        void setHttpCallback(HttpCallback cb) {
            http_callback_ = std::move(cb);
        }

        void start();

    private:
        void onMessage(const TinyMuduo::TcpServer::TcpConnectionPtr &conn,
                       TinyMuduo::Buffer *buffer, TinyMuduo::TimeStamp timestamp);

        void onConn(const TinyMuduo::TcpConnection::TcpConnectionPtr &conn);

        void onRequest(const TinyMuduo::TcpConnection::TcpConnectionPtr &conn, const TinyHttp::HttpRequest &request);

        TinyMuduo::TcpServer server_;

        HttpCallback http_callback_;
    };

}

#endif //TINYMUDUO_HTTPSERVER_H
