//
// Created by 杨丰硕 on 2023/1/15.
//
#include <gtest/gtest.h>
#include "../net/TcpConnection.h"
#include "../base/Buffer.h"
#include "../net/SockAddress.h"
#include "../reactor/EventLoop.h"
#include "../base/Timestamp.h"
#include "../net/TcpServer.h"

using namespace TinyMuduo;

class EchoServer {
public:
    EchoServer(EventLoop *loop, const SockAddress &addr, uint32_t thread_size);

    ~EchoServer() = default;

    void start();

private:
    void onMassege(const TcpServer::TcpConnectionPtr &conn);
    void onConnection(const TcpServer::TcpConnectionPtr &conn, Buffer *buffer, TimeStamp rettime);

    EventLoop *loop_;
    TcpServer server_;
};

EchoServer::EchoServer(EventLoop *loop, const SockAddress &addr, uint32_t thread_size):
    loop_(loop),
    server_("echo_server", "0.0.0.0", 12334, thread_size, loop_, "echo_loop_pool"){
    server_.setIoThreadNumber(thread_size);
}

void EchoServer::start() {
    server_.start();
}

void EchoServer::onConnection(const TcpServer::TcpConnectionPtr &conn, Buffer *buffer, TimeStamp rettime) {
    auto local_addr = conn->get

}

void EchoServer::onMassege(const TcpServer::TcpConnectionPtr &conn) {

}



TEST(SERVER_TEST, SIMPLE_SERVER_TEST) {
    EventLoop loop;
    SockAddress address(12334);
    EchoServer echo_server(&loop, address, 30);
    loop.loop(-1);      // 启动,之后观察

}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

