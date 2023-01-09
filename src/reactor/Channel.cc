//
// Created by 杨丰硕 on 2023/1/9.
//
#include "Channel.h"

using namespace TinyMuduo;

const uint32_t Channel::NULL_EVENT = 0;
const uint32_t Channel::READ_EVENT = EPOLLIN | EPOLLPRI; // 带外数据到底指的是什么??
const uint32_t Channel::WRITE_EVENT = EPOLLOUT;

Channel::Channel(EventLoop *eventLoop):owner_loop_(eventLoop) {}

Channel::Channel():socket_() {}

Channel::~Channel() {}

void Channel::handleEvent() {
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