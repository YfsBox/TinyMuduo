//
// Created by 杨丰硕 on 2023/1/13.
//
#include <string.h>
#include "LoopThreadPool.h"
#include "EventLoopThread.h"
#include "../reactor/EventLoop.h"

using namespace TinyMuduo;

LoopThreadPool::LoopThreadPool(EventLoop *ownerloop, const std::string name):
    is_started_(false),
    owner_loop_(ownerloop),
    next_loop_num_(0),
    pool_name_(name),
    thread_size_(0){

}

LoopThreadPool::~LoopThreadPool() {
    if (is_started_) {
        stop();
    }
}

EventLoop *LoopThreadPool::getNextLoop() {
    owner_loop_->assertInThisThread();  // 只有owner thread才可以调用该方法

    if (!is_started_ || thread_size_ == 0) {
        return owner_loop_;
    }

    EventLoop *next_loop = sub_loops_[next_loop_num_].get();
    next_loop_num_ = (next_loop_num_ + 1) % thread_size_;

    return next_loop;
}

void LoopThreadPool::start() {
    // 初始化thread及其loop
    owner_loop_->assertInThisThread();  // 必须只有owner_loop才可以调用该函数
    is_started_ = true;
    next_loop_num_ = 0;

    loop_threads_.reserve(thread_size_);
    sub_loops_.reserve(thread_size_);

    for (size_t i = 0; i < thread_size_; ++i) {
        char name[64];
        memset(name, 0, sizeof(name));
        sprintf(name, "#%s:%zu", pool_name_.c_str(), i);
        loop_threads_.emplace_back(std::make_unique<EventLoopThread>(name));
        sub_loops_.emplace_back(loop_threads_[i]->startAndGetEventLoop());
    }
}

void LoopThreadPool::stop() {
    owner_loop_->assertInThisThread();

    loop_threads_.clear();
    sub_loops_.clear();
    is_started_ = false;
}



