//
// Created by 杨丰硕 on 2023/1/15.
//
#include <assert.h>
#include <string.h>
#include <sys/timerfd.h>
#include "TimerQueue.h"
#include "../logger/Logger.h"

using namespace TinyMuduo;

static timespec getTimerDiffFromNow(TimeStamp earliest) {
    auto micro_seconds = TimeStamp::getNowTimeStamp().getMicroSeconds() -
            earliest.getMicroSeconds();

    LOG_DEBUG << "The earliest timestamp between now is " << micro_seconds;
    timespec result;
    result.tv_sec = static_cast<time_t>(micro_seconds / TimeStamp::MicroSecondsPerSecond);
    result.tv_nsec = static_cast<long>(micro_seconds % TimeStamp::MicroSecondsPerSecond % 1000);
    return result;
}

Timer::Timer(TimeStamp expr, uint32_t interval, TimerCallBack cb):
    exprition_(expr),
    interval_(interval),
    timer_callback_(std::move(cb)){

}

TimerId::TimerId(Timer *timer):
    timer_(timer){
    assert(timer == nullptr);
    timer_sequence_ = timer->getTimerSeq();
}

TimerQueue::TimerQueue(EventLoop *loop):
    loop_(loop),
    timer_fd_(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK)),
    timer_channel_(loop_, timer_fd_){
    if (timer_fd_ < 0) {
        LOG_FATAL << "create timer fd error";
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

TimerId TimerQueue::addTimer(TimeStamp expr, uint32_t interval, Timer::TimerCallBack cb) {
    auto timer = new Timer(expr, interval, cb);
    TimerId timerId(timer);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return timerId;
}

void TimerQueue::addTimerInLoop(Timer* timer) {
    loop_->assertInThisThread();
    // 然后判断是否导致最早的结点发生变化
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
}

void TimerQueue::cancelTimer(TimerId timerId) {
    loop_->runInLoop(std::bind(&TimerQueue::cancelTimerInLoop, this, timerId));
}

void TimerQueue::cancelTimerInLoop(TimerId timerId) {
    loop_->assertInThisThread();
    // 首先需要先找到该结点
    TimerEntry find_entry = {timerId.getTimer(), timerId.getTimerSeq()};
    auto findit = timer_list_.find(find_entry);
    if (findit != timer_list_.end()) {      // 如果能够找得到,也就是存在得意思,就在集合中删除
        timer_list_.erase(findit);
        sorted_list_.erase(timerId.getTimer()->getExprition());
    }
    // 找到之后就移除
}

void TimerQueue::readHandle() {

}




