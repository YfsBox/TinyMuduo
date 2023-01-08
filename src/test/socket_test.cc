//
// Created by 杨丰硕 on 2023/1/8.
//
#include "../net/SockAddress.h"
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


}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

