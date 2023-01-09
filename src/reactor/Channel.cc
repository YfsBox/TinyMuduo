//
// Created by 杨丰硕 on 2023/1/9.
//
#include "Channel.h"

using namespace TinyMuduo;

const uint32_t Channel::NULL_EVENT = 0;
const uint32_t Channel::READ_EVENT = EPOLLIN | EPOLLPRI; // 带外数据到底指的是什么??
const uint32_t Channel::WRITE_EVENT = EPOLLOUT;

Channel::Channel():socket_() {}

Channel::~Channel() {}

