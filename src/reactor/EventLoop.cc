//
// Created by 杨丰硕 on 2023/1/8.
//
#include <assert.h>
#include "EventLoop.h"
#include "Epoller.h"
#include "Channel.h"
#include "../base/Thread.h"

using namespace TinyMuduo;

EventLoop::EventLoop():
        is_looping_(false),
        is_quit_(false),
        thread_id_(CurrThreadSpace::CurrThreadId),
        epoller_(std::make_unique<Epoller>()){
    if (CurrThreadSpace::LoopInThisThread == nullptr) {
        CurrThreadSpace::LoopInThisThread = this;
    } else {    // 否则就应该出错
        assert(1);
    }
}

EventLoop::~EventLoop() {
}

void EventLoop::loop(int timeout) {
    is_looping_ = true;
    is_quit_ = false;

    while (!is_quit_) {
        active_channels_.clear();
        auto epoll_wait_cnt = epoller_->epoll(timeout, active_channels_);

        for (auto channel : active_channels_) {
            channel->handleEvent();     // 处理返回的事件
        }
    }

    is_looping_ = false;
}

void EventLoop::quit() {
    if (is_looping_ && !is_quit_) {
        is_quit_ = true;
    }
}

