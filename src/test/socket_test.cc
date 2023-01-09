//
// Created by 杨丰硕 on 2023/1/8.
//
#include "../base/Thread.h"
#include "../net/SockAddress.h"
#include "../net/Socket.h"
#include <gtest/gtest.h>

using namespace TinyMuduo;

TEST(SOCKET_TEST, SOCKADDR_TEST) {
    // 只指定port,此时的ip应该是0.0.0.0
    SockAddress addr_port(10001);
    ASSERT_EQ(addr_port.getPort(), 10001);
    ASSERT_EQ(addr_port.getIp(), "0.0.0.0");
    // 同时指定port和ip
    std::string ip = "1.2.234.12";
    uint16_t port2 = 10002;
    SockAddress addr_port_and_ip(ip, port2);
    ASSERT_EQ(addr_port_and_ip.getIp(), ip);
    ASSERT_EQ(addr_port_and_ip.getPort(), port2);
    // 借助sockaddr_in进行构造
    auto tmp_sockaddr_in = addr_port_and_ip.getSockAddrIn();
    SockAddress addr_sockaddr_in(tmp_sockaddr_in);
    ASSERT_EQ(addr_sockaddr_in.getIp(), ip);
    ASSERT_EQ(addr_sockaddr_in.getPort(), port2);

}

TEST(SOCKET_TEST, BASIC_SOCKET_TEST) {
    using SocketShptr = std::shared_ptr<Socket>;
    // 创建一堆socket
    std::vector<SocketShptr> connect_sockets;
    std::vector<SocketShptr> listen_socket;
    for (size_t i = 0; i < 50; ++i) {
        connect_sockets.push_back(std::make_shared<Socket>());
        listen_socket.push_back(std::make_shared<Socket>());
        // std::cout << connect_sockets[i]->getFd() << " and " << listen_socket[i]->getFd() << "\n";
        ASSERT_NE(connect_sockets[i]->getFd(), -1);     // 不等于-1
        ASSERT_NE(listen_socket[i]->getFd(), -1);
    }
    // 从中指定一个server尝试监听并等待连接
    std::atomic<bool> is_exit = false;
    SocketShptr server_socket = listen_socket[0];
    uint16_t base_port = 12240;
    SockAddress serverAddress(base_port);     // server的地址
    auto server_func = [&]() {
        int bind_ret = server_socket->bindSockAddress(serverAddress);
        ASSERT_TRUE(bind_ret >= 0);

        int listen_ret = server_socket->listen();
        ASSERT_TRUE(listen_ret >= 0);    // 必须大于0

        int accept_cnt = 50; // 目标是成功接收50个client
        while (accept_cnt > 0) {
            SockAddress peerAddress;
            int accept_fd = server_socket->accept(&peerAddress);
            ASSERT_TRUE(accept_fd >= 0);
            accept_cnt--;
        }
        printf("accept test in server ok!");
    };

    Thread server(server_func);
    server.startThread(); // 启动server

    auto client_func = [&](uint16_t client_port, SocketShptr client_socket) {
        SockAddress clientAddress(client_port);
        int bind_ret = client_socket->bindSockAddress(clientAddress);
        ASSERT_TRUE(bind_ret >= 0);

        int connect_ret = client_socket->connect(&serverAddress);
        ASSERT_TRUE(connect_ret >= 0);
    };
    using ThreadShptr = std::shared_ptr<Thread>;
    std::vector<ThreadShptr> client_threads;
    for (size_t i = 0; i < 50; ++i) {
        client_threads.push_back(std::make_shared<Thread>(std::bind(client_func, base_port + i + 1, connect_sockets[i])));
    }
    for (auto &client : client_threads) {
        client->startThread();
    }
    // 等待一会
    sleep(2);
    for (auto &client : client_threads) {
        client->joinThread();
    }
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

