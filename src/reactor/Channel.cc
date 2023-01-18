//
// Created by 杨丰硕 on 2023/1/9.
//
#include "Channel.h"
#include "../logger/Logger.h"
#include "../net/Socket.h"

using namespace TinyMuduo;

const uint32_t Channel::NULL_EVENT = 0;
const uint32_t Channel::READ_EVENT = EPOLLIN | EPOLLPRI; // 带外数据到底指的是什么??
const uint32_t Channel::WRITE_EVENT = EPOLLOUT;

Channel::Channel(EventLoop *eventLoop, int fd):
    in_loop_(false),
    state_(ChannelState::newChannel),
    owner_loop_(eventLoop),
    event_(NULL_EVENT),
    revent_(NULL_EVENT),
    fd_(fd),
    tied_(false){}

Channel::Channel():
    in_loop_(false),
    state_(ChannelState::newChannel),
    owner_loop_(nullptr),
    event_(NULL_EVENT),
    revent_(NULL_EVENT),
    fd_(-1) {}

Channel::~Channel() {
    // LOG_DEBUG << "the Channel " << reinterpret_cast<uintptr_t>(this) << " destory";
}

void Channel::handleEvent() {           // 这里暂时不考虑悬空引用的情况
    if(tied_) {
        auto guard = tie_.lock();
        if(guard) {
            handleEventWithGuard();
        }
    } else {
        handleEventWithGuard();
    }
    // handleEventWithGuard();
}

void Channel::handleEventWithGuard() {
    /*LOG_DEBUG << reinterpret_cast<uintptr_t>(this) << " revent " << (revent_ & READ_EVENT)
    << " and " << (revent_ & EPOLLERR);*/
    if (revent_ & READ_EVENT) {     // 可读事件
        if (read_callback_) {
            // LOG_DEBUG << reinterpret_cast<uintptr_t>(this) << " call the read callback";
            read_callback_();
        }
    }
    if (revent_ & WRITE_EVENT) {     // 可写事件
        if (write_callback_) {
            write_callback_();
        }
    }
    if (revent_ & EPOLLERR) {
        if (error_callback_) {
            // LOG_DEBUG << reinterpret_cast<uintptr_t>(this) << " call the error callback";
            error_callback_();
        }
    }
    if ((revent_ & EPOLLHUP) && !(revent_ & EPOLLIN)) {
        if (close_callback_) {
            close_callback_();
        }
    }
}

void Channel::tie(const std::shared_ptr<void> &obj) {
    tie_ = obj;
    tied_ = true;
}