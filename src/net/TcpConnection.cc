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
                             channel_(std::make_unique<Channel>(loop_, fd)),
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
    if (!channel_->isReadable()) {
        return;
    }
    int error_no;
    ssize_t retn = read_buffer_.read(channel_->getFd(), &error_no);
    if (retn > 0) {     //正常的读取
        // onMessage
        message_callback_(shared_from_this());
    } else if (retn == 0) {
        closeHandle();
    } else {
        errorHandle();
    }
}

void TcpConnection::writeHandle() {
    if (!channel_->isWritable()) {
        return;
    }
    int error_no;
    ssize_t retn = write_buffer_.write(channel_->getFd(), &error_no);
    if (retn > 0) {
        write_buffer_.retrieve(retn);       // 移动write index
        if (write_buffer_.getReadableSize() == 0) {     // 如果改写的东西都已经写完了
            channel_->disableWritable();
            // 还需要考虑WriteComplete的情况及其回调函数
        }
    } else {
        errno = error_no;
        LOG_ERROR << "write error in write handle";
    }

}

void TcpConnection::closeHandle() {
    setState(Closed);

    channel_->setDisable();
    close_callback_(shared_from_this());
}

void TcpConnection::errorHandle() {
    LOG_ERROR << "The error handle in the TcpConnection";
}

void TcpConnection::establish() {
    if (state_ == Connecting) {
        channel_->setReadable();
    }
    connection_callback_(shared_from_this());
}

void TcpConnection::destroy() {
    channel_->setDisable();
    channel_->remove();
    connection_callback_(shared_from_this());
}

