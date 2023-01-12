//
// Created by 杨丰硕 on 2023/1/9.
//
#include "Channel.h"
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
    fd_(fd) {}

Channel::Channel():
    in_loop_(false),
    state_(ChannelState::newChannel),
    owner_loop_(nullptr),
    event_(NULL_EVENT),
    revent_(NULL_EVENT),
    fd_(-1) {}

Channel::~Channel() = default;

void Channel::handleEvent() {           // 这里暂时不考虑悬空引用的情况
    // 根据handle来判断
    if (revent_ & READ_EVENT) {     // 可读事件
        if (read_callback_) {
            read_callback_();
        }
    } else if (revent_ & WRITE_EVENT) {     // 可写事件
        if (write_callback_) {
            write_callback_();
        }
    }
}