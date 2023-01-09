//
// Created by 杨丰硕 on 2023/1/8.
//
#include "Thread.h"
#include "Utils.h"

using namespace TinyMuduo;

namespace TinyMuduo::CurrThreadSpace {

    thread_local pid_t CurrThreadId = -1;

    thread_local EventLoop* LoopInThisThread = nullptr;

    void setCurrThreadId() {
        if (CurrThreadId == -1) {
            CurrThreadId = static_cast<pid_t>(gettid());
        }
    }
}

std::atomic<uint32_t> TinyMuduo::Thread::created_thread_num_ = 0;

Thread::Thread(ThreadFunc func):
    is_join_(false),
    is_start(false),
    thread_id_(0){
    created_thread_num_++;
    thread_name_ = getDefaultName();
    setFunc(func);
}

Thread::Thread(Thread::ThreadFunc func, const std::string &name):
    is_join_(false),
    is_start(false),
    thread_id_(0),
    thread_name_(name){
    created_thread_num_++;
    setFunc(func);
}

Thread::~Thread() {
    if (is_start && !is_join_) {
        thread_->detach();
    }
}

void Thread::startThread() {
    if (!is_start) {
        is_join_ = false;
        is_start = true;
        thread_ = std::make_unique<std::thread>([&](){
            CurrThreadSpace::setCurrThreadId();
            thread_id_ = CurrThreadSpace::CurrThreadId;
            main_func_();
        });
    }
}

void Thread::joinThread() {
    if (!is_join_ && is_start) {        // join和start可以用来描述线程的状态
        is_join_ = true;
        is_start = false;
        if (thread_->joinable()) {
            printf("The thread %s will joined\n", thread_name_.c_str());
            thread_->join();
        }
    }
}

void Thread::setFunc(ThreadFunc func) {
    main_func_ = func;
}

std::string Thread::getDefaultName() {
    int num = created_thread_num_;
    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "thread_%d", num);
    return buf;
}

uint32_t Thread::getCreatedThreadNum() {
    return created_thread_num_;
}
