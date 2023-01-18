//
// Created by 杨丰硕 on 2023/1/15.
//
#include <cassert>
#include <cstring>
#include <sys/timerfd.h>
#include "TimerQueue.h"
#include "../logger/Logger.h"

using namespace TinyMuduo;

static timespec getTimerDiffFromNow(TimeStamp earliest) {
    auto micro_seconds = earliest.getMicroSeconds() -
            TimeStamp::getNowTimeStamp().getMicroSeconds();

    // LOG_DEBUG << "The earliest timestamp between now is " << micro_seconds;
    timespec result;
    result.tv_sec = static_cast<time_t>(micro_seconds / TimeStamp::MicroSecondsPerSecond);
    result.tv_nsec = static_cast<long>((micro_seconds % TimeStamp::MicroSecondsPerSecond) * 1000);
    return result;
}

Timer::Timer(TimeStamp expr, uint32_t interval, TimerCallBack cb):
    exprition_(expr),
    interval_(interval),
    timer_callback_(std::move(cb)){

}

TimerId::TimerId(Timer *timer):
    timer_(timer){
    //assert(timer == nullptr);
    if (timer == nullptr) {
        LOG_ERROR << "The timer is null";
    }
    timer_sequence_ = timer->getTimerSeq();
}

TimerQueue::TimerQueue(EventLoop *loop):
    loop_(loop),
    timer_fd_(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK)),
    timer_channel_(loop_, timer_fd_){
    if (timer_fd_ < 0) {
        LOG_FATAL << "create timer fd error";
        assert(1);
    } else {
        timer_channel_.setReadCallBack(std::bind(&TimerQueue::readHandle, this));
        timer_channel_.setReadable();
    }
}

TimerQueue::~TimerQueue() {
    timer_channel_.setDisable();
    timer_channel_.remove();
    ::close(timer_fd_);
}

void TimerQueue::resetTimerFd(TimeStamp expr) {
    itimerspec old_time, new_time;
    memset(&old_time, 0, sizeof(old_time));
    memset(&new_time, 0, sizeof(new_time));
    // 计算时间间隔
    new_time.it_value = getTimerDiffFromNow(expr);
    if (::timerfd_settime(timer_fd_, 0, &new_time, &old_time) < 0) {
        LOG_ERROR << "set timefd error, the new time is {" << new_time.it_value.tv_sec
                  << "," << new_time.it_value.tv_nsec;
    }
}

TimerId TimerQueue::addTimer(TimeStamp expr, uint32_t interval, Timer::TimerCallBack cb) {
    auto timer = new Timer(expr, interval, cb);
    TimerId timerId(timer);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return timerId;
}

void TimerQueue::addTimerInLoop(Timer* timer) {
    loop_->assertInThisThread();
    // 然后判断是否导致最早的结点发生变化
    // LOG_DEBUG << "push the timer which at " << timer->getExprition().getTimeFormatString();
    auto expr = timer->getExprition();
    bool earliest_changed = (sorted_list_.empty() ||
            expr < sorted_list_.begin()->first);
    // 插入到两个队列中去
    auto seq = timer->getTimerSeq();
    TimerPtr timer_ptr(timer);
    timer_list_.insert({timer_ptr.get(), seq});
    sorted_list_.insert({expr, std::move(timer_ptr)});
    // 如果有变化就重新调整该描述符
    if (earliest_changed) {
        resetTimerFd(expr);
    }
    /*LOG_DEBUG << "the set size is " << timer_list_.size() << " and the earliest is "
    << sorted_list_.begin()->first.getTimeFormatString();*/
}

void TimerQueue::cancelTimer(TimerId timerId) {
    loop_->runInLoop(std::bind(&TimerQueue::cancelTimerInLoop, this, timerId));
}

void TimerQueue::cancelTimerInLoop(TimerId timerId) {
    loop_->assertInThisThread();
    // 首先需要先找到该结点
    auto findit = timer_list_.find(timerId.getTimer());
    if (findit != timer_list_.end()) {      // 如果能够找得到,也就是存在得意思,就在集合中删除
        timer_list_.erase(findit);
        sorted_list_.erase(timerId.getTimer()->getExprition());
    }
    // 找到之后就移除
}

void TimerQueue::readTimerFd() const {
    uint64_t num;       // 这个地方得是2个字节的东西，否则会读取错误
    ssize_t ret = ::read(timer_fd_, &num, sizeof(num));
    if (ret < 0) {
        LOG_ERROR << "read timer fd error in read handle";
    }
}

std::vector<TimerQueue::SortedEntry> TimerQueue::getExprTimers(TimeStamp now) {
    std::vector<SortedEntry> exprs;
    auto end = sorted_list_.lower_bound(now);
    // 拷贝到exprs中,unique_ptr的所有权就被转移到vector
    //std::copy(sorted_list_.begin(), end, std::back_inserter(exprs));
    for (auto sorted_it = sorted_list_.begin(); sorted_it != end; ++sorted_it) {
        exprs.emplace_back(sorted_it->first, std::move(sorted_it->second));
    }
    sorted_list_.erase(sorted_list_.begin(), end);
    // 然后在timer_list中移除
    for (auto &[timestamp, timerptr]: exprs) {
        auto findit = timer_list_.find(timerptr.get());
        if (findit != timer_list_.end()) {
            timer_list_.erase(findit);
        }
    }
    return exprs;
}

void TimerQueue::readHandle() {     // 确保只有timer fd所属的一个线程运行handle
    // 首先读取timer fd
    // LOG_DEBUG << "get the read handle";
    readTimerFd();
    auto now_timestamp = TimeStamp::getNowTimeStamp();
    // 获取其中过期了的结点
    auto exprs = getExprTimers(now_timestamp);
    // 将其中过期的结点都run一下
    for (auto &[stamp, timer]: exprs) {
        timer->run();
    }

    if (!sorted_list_.empty()) {
        auto new_earliest = sorted_list_.begin()->first;
        resetTimerFd(new_earliest);
    }   // 重新调整earliest

}




