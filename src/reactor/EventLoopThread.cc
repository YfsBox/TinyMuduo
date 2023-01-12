//
// Created by 杨丰硕 on 2023/1/12.
//
#include "EventLoop.h"
#include "EventLoopThread.h"

using namespace TinyMuduo;

EventLoopThread::EventLoopThread(const std::string &name):
        is_looping_(false),
        loop_(nullptr),
        thread_(std::make_unique<Thread>
                (std::bind(&EventLoopThread::threadLoopFunc, this), name)){

}

EventLoopThread::~EventLoopThread() {
    if (is_looping_) {
        is_looping_ = false;
        loop_->quit();
        thread_->joinThread();
    }
}

EventLoop *EventLoopThread::startAndGetEventLoop() {
    if (is_looping_) {
        return loop_;
    }
    // 如果没有启动过,就先启动并且等待。
    thread_->startThread();
    std::unique_lock<std::mutex> uniqueLock(mutex_);
    while (loop_ == nullptr) {
        condition_.wait(uniqueLock);
    }
    return loop_;
}


void EventLoopThread::threadLoopFunc() {
    EventLoop eventloop;
    is_looping_ = true;
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        loop_ = &eventloop;
        condition_.notify_one();
    }
    loop_->loop(0);     // 这个loop先暂
    // 退出之后
    is_looping_ = false;
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        loop_ = nullptr;
    }
}

