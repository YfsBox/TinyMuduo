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
    // channel_->setDisable();
    // LOG_INFO << "The TcpConnection " << channel_->getFd() << " removed";
}
// handle本身就是在io线程中进行的
// 下面的handle相关的，都是需要保证处于io线程中运行的，外部要调用的话必须要runInLoop
void TcpConnection::readHandle() {
    if (!channel_->isReadable()) {
        return;
    }
    int error_no;
    ssize_t retn = read_buffer_.read(channel_->getFd(), &error_no);
    if (retn > 0) {     //正常的读取
        // onMessage
        message_callback_(shared_from_this(), &read_buffer_, TimeStamp::getNowTimeStamp());
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
            if (state_ == ConnectionState::DisConnecting) {
                shutdownInLoop();       // 执行未完全完成的关闭状态
            }
        }
    } else {
        errno = error_no;
        LOG_ERROR << "write error in write handle";
    }

}

void TcpConnection::closeHandle() {         // 只有被动关闭才是最彻底的关闭方式
    // LOG_INFO << "close a tcp connection, the fd is " << channel_->getFd(); /*<< " the use cnt is "
    //<< shared_from_this().use_count() << " and " << shared_from_this().use_count();*/
    setState(ConnectionState::DisConnected);
    // 由于该connection首先会在Server中的map里移除,会导致引用计数为0,而后续的destroy是一个
    // 异步的操作,需要将生命周期延长到destroy执行完，所以在这里将其引用计数暂且+1
    channel_->setDisable();
    // channel_->remove();         // 需要移除channel吗
    TcpConnectionPtr conn(shared_from_this());
    // LOG_DEBUG << "the conn use cnt is " << conn.use_count();
    close_callback_(conn);
}

void TcpConnection::errorHandle() {
    LOG_ERROR << "The error handle in the TcpConnection";
}

void TcpConnection::establish() {       // 该函数也要保证放在该loop所处的线程中处理
    if (state_ == Connecting) {
        setState(ConnectionState::Connected);
        // channel_->tie(shared_from_this());
        channel_->setReadable();
    }
    connection_callback_(shared_from_this());
}

void TcpConnection::destroy() {     //  这个函数需要保证放在TcpConnection所处的loop中
    channel_->setDisable();
    channel_->remove();
    connection_callback_(shared_from_this());
}

void TcpConnection::send(const std::string &content) {
    send(content.c_str(), content.size());
}

void TcpConnection::send(const char *content, size_t len) {   // send的实际操作中可能涉及到IO，因此保证其放入到IO线程中操作
    if (state_ == ConnectionState::Connected) {
        if (loop_->isOuterThread()) {
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, content, len));
        } else {
            sendInLoop(content, len);
        }
    }
}

void TcpConnection::sendInLoop(const char *content, size_t len) {
    // 首先判断状态
    if (state_ == ConnectionState::DisConnected) {
        return;
    }
    // 首先尝试直接通过write进行写
    ssize_t wroten = 0, remain = len;
    bool has_error = false;
    if (!channel_->isWritable() && write_buffer_.getReadableSize() == 0) {
        wroten = ::write(channel_->getFd(), content, len);
        if (wroten >= 0) {
            remain -= wroten;
        } else {
            LOG_ERROR << "the send in loop error";
            has_error = true;
        }
    }
    if (remain != 0 && !has_error) {        // 还有剩余的话,就先输出到buffer上
        write_buffer_.append(content + wroten, remain);
        if (!channel_->isWritable()) {
            channel_->setWritable();
        }
    }
}
// 外部调用
void TcpConnection::shutdown() {        // 通常在服务端进行半关闭
    // 要考虑到写操作的情况，如果当前不处于writable
    setState(ConnectionState::DisConnecting);       // 设置成Connecting,writeHandle中会有相应的判断
    loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
}

void TcpConnection::shutdownInLoop() {
    if (!channel_->isWritable()) {
        Socket::shutdownWrite(channel_->getFd());
    }
}



