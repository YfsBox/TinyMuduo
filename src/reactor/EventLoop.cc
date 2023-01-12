//
// Created by 杨丰硕 on 2023/1/8.
//
#include <assert.h>
#include "EventLoop.h"
#include "Epoller.h"
#include "Channel.h"
#include "../logger/Logger.h"
#include "../base/Timestamp.h"
#include "../base/Thread.h"

using namespace TinyMuduo;

EventLoop::EventLoop():
        is_looping_(false),
        is_quit_(false),
        thread_id_(CurrThreadSpace::getCurrThreadId()),
        epoller_(std::make_unique<Epoller>()){
    if (CurrThreadSpace::LoopInThisThread == nullptr) {
        CurrThreadSpace::LoopInThisThread = this;
    } else {    // 否则就应该出错
        assert(1);
    }
}

EventLoop::~EventLoop() {
}

void EventLoop::updateChannel(Channel *channel) {
    epoller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    epoller_->removeChannel(channel);
}


void EventLoop::loop(int timeout) {
    is_looping_ = true;
    is_quit_ = false;

    while (!is_quit_) {
        active_channels_.clear();
        auto return_time = epoller_->epoll(-1, active_channels_);
        LOG_DEBUG << "epoller epoll return at time " << return_time.getTimeFormatString()
        << " and the active channels has" << active_channels_.size();
        for (auto channel : active_channels_) {
            LOG_DEBUG << "handle the channel whose fd is " << channel->getFd();
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

