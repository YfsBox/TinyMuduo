//
// Created by 杨丰硕 on 2023/1/8.
//
#include <assert.h>
#include <sys/eventfd.h>
#include "EventLoop.h"
#include "Epoller.h"
#include "Channel.h"
#include "../logger/Logger.h"
#include "../base/Timestamp.h"
#include "../base/Thread.h"
#include "../reactor/TimerQueue.h"

using namespace TinyMuduo;

EventLoop::EventLoop():
        is_looping_(false),
        is_quit_(false),
        thread_id_(CurrThreadSpace::getCurrThreadId()),
        epoller_(std::make_unique<Epoller>()),
        timer_queue_(std::make_unique<TimerQueue>(this)),
        wakeup_fd_(createWakeupFd()),
        wakeup_channel_(std::make_unique<Channel>(this, wakeup_fd_)){
    if (CurrThreadSpace::LoopInThisThread == nullptr) {
        CurrThreadSpace::LoopInThisThread = this;
    } else {    // 否则就应该出错
        LOG_ERROR << "The Loop in this thread is not nullptr";
        assert(1);
    }
    wakeup_channel_->setReadCallBack(std::bind(&EventLoop::wakeupHandle, this));
    wakeup_channel_->setReadable();
}

EventLoop::~EventLoop() {
    wakeup_channel_->setEvent(Channel::NULL_EVENT);
    wakeup_channel_->remove();
    ::close(wakeup_fd_);
    CurrThreadSpace::LoopInThisThread = nullptr;
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
        auto return_time = epoller_->epoll(defaultTimeoutMs, active_channels_);
        LOG_DEBUG << "epoller epoll return at time " << return_time.getTimeFormatString()
        << " and the active channels has " << active_channels_.size();
        for (auto channel : active_channels_) {
            channel->handleEvent();     // 处理返回的事件
        }
        doQueuedFunctors();
    }

    is_looping_ = false;
}

void EventLoop::quit() {
    if (is_looping_ && !is_quit_) {
        is_quit_ = true;
    }
}

int EventLoop::createWakeupFd() {
    int ret = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (ret < 0) {
        LOG_ERROR << "The event fd is error in Eventloop";
    }
    return ret;
}

void EventLoop::wakeup() {          // 外部调用和内部调用都可以
    uint64_t num = 1;
    auto ret = ::write(wakeup_fd_, &num, sizeof(num));
    if (ret < 0) {
        LOG_ERROR << "Wakeup the epoller by write wakeup_fd error";
    }
}

void EventLoop::wakeupAndQuit() {
    quit();
    wakeup();
}

void EventLoop::assertInThisThread() {          // 如果是外部线程调用的该loop中的方法
    if (isOuterThread()) {
        LOG_FATAL << "run not in the eventloop's thread";
    }
}

void EventLoop::wakeupHandle() {
    uint64_t num;
    auto ret = ::read(wakeup_fd_, &num, sizeof(num));
    if (ret < 0) {
        LOG_ERROR << "Handle the epoller' waker error";
    }
}

void EventLoop::doQueuedFunctors() {
    assertInThisThread();   // 不可以在外部被调用

    std::deque<QueuedFunctor> tmp_queue;
    {
        std::lock_guard<std::mutex> lockGuard(mutex_);
        tmp_queue.swap(queued_list_);       // 交换过来
    }
    // 执行queue中的函数
    for (const auto &task : tmp_queue) {
        task();
    }
}

size_t EventLoop::getQueueSize() {
    std::lock_guard<std::mutex> lockGuard(mutex_);
    return queued_list_.size();
}

void EventLoop::runInLoop(QueuedFunctor func) {
    if (func == nullptr) {      // 不可以为空
        return;
    }
    if (isOuterThread()) {
        // 加入到队列里
        {
            std::lock_guard<std::mutex> lockGuard(mutex_);
            queued_list_.push_back(std::move(func));
        }
        wakeup(); // 唤醒loop,loop中将会处理被加入到队列中的函数
    } else {        // 直接运行
        func();
    }
}

TimerId EventLoop::runTimertaskAfter(uint64_t delay, TimerTask task) {
    TimeStamp timestamp(TimeStamp::getNowTimeStamp().getMicroSeconds() + delay);
    return runTimertaskAt(timestamp, std::move(task));
}

TimerId EventLoop::runTimertaskAt(TimeStamp timestamp, TimerTask task) {
    return timer_queue_->addTimer(timestamp, 0, std::move(task));
}



