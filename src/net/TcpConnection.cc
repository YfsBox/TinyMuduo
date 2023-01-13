//
// Created by 杨丰硕 on 2023/1/9.
//
#include "TcpConnection.h"
#include "../net/Socket.h"
#include "../logger/Logger.h"
#include "../reactor/Channel.h"

using namespace TinyMuduo;

TcpConnection::TcpConnection(EventLoop *loop,
                             const std::string &name, int fd,
                             const SockAddress &local,const SockAddress &peer):
                             loop_(loop),
                             state_(ConnectionState::Connecting),
                             conn_name_(name),
                             socket_(std::make_unique<Socket>(fd)),
                             local_addr_(local),
                             peer_addr_(peer) {
    channel_->setReadCallBack(std::bind(&TcpConnection::readHandle, this));
    channel_->setWriteCallBack(std::bind(&TcpConnection::writeHandle, this));
    channel_->setCloseCallBack(std::bind(&TcpConnection::closeHandle, this));
    channel_->setErrorCallBack(std::bind(&TcpConnection::errorHandle, this));
}

TcpConnection::~TcpConnection() {
    channel_->setEvent(Channel::NULL_EVENT);
}

void TcpConnection::readHandle() {
    char buf[65536];
    ssize_t retn = ::read(channel_->getFd(), buf, sizeof(buf));
    if (retn > 0) {     //正常的读取
        // onMessage
    } else if (retn == 0) {
        closeHandle();
    } else {
        errorHandle();
    }
}

void TcpConnection::writeHandle() {


}

void TcpConnection::closeHandle() {

}

void TcpConnection::errorHandle() {
    LOG_ERROR << "The error handle in the TcpConnection";
}

