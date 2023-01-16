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
#include <functional>

using namespace TinyMuduo;

class EchoServer {
public:
    EchoServer(EventLoop *loop, const SockAddress &addr, uint32_t thread_size);

    ~EchoServer() = default;

    void start();

private:
    void onConnection(const TcpServer::TcpConnectionPtr &conn);
    void onMassege(const TcpServer::TcpConnectionPtr &conn, Buffer *buffer, TimeStamp rettime);

    EventLoop *loop_;
    TcpServer server_;
};

EchoServer::EchoServer(EventLoop *loop, const SockAddress &addr, uint32_t thread_size):
    loop_(loop),
    server_("echo_server", addr.getIp(), addr.getPort(), thread_size, loop_, "echo_loop_pool"){
    server_.setIoThreadNumber(thread_size);
    server_.setConnCb(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCb(std::bind(&EchoServer::onMassege, this, std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3));
}

void EchoServer::start() {
    server_.start();
}

void EchoServer::onMassege(const TcpServer::TcpConnectionPtr &conn, Buffer *buffer, TimeStamp rettime) {
    std::string msg(buffer->retrieveAsStringAll());
    LOG_INFO << conn->getName() << " recv " << msg.size() << " bytes at " << rettime.getTimeFormatString();
    LOG_DEBUG << "Msg is:\n" << msg;
    if (msg == "exit") {
        LOG_DEBUG << "send bye and shutdown";
        conn->send("bye\n");
        conn->shutdown();
    }
    if (msg == "quit") {
        LOG_DEBUG << "quit the loop";
        loop_->quit();
    }
    conn->send(msg);
}


void EchoServer::onConnection(const TcpServer::TcpConnectionPtr &conn) {
    auto local_addr = conn->getLocalAddr();
    auto peer_addr = conn->getPeerAddr();
    std::string conn_state = "DOWN";
    if (conn->isConnected()) {
        conn_state = "UP";
    }
    LOG_INFO << local_addr.getIp() << ":" << local_addr.getPort() << " and "
             << peer_addr.getIp() << ":" <<peer_addr.getPort() << " " << conn_state;

}



TEST(SERVER_TEST, SIMPLE_SERVER_TEST) {
    EventLoop loop;
    SockAddress address(12334);
    EchoServer echo_server(&loop, address, 30);
    echo_server.start();
    loop.loop(-1);      // 启动,之后观察

}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

