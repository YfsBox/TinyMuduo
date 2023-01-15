//
// Created by 杨丰硕 on 2023/1/15.
//
#include <assert.h>
#include "TimerQueue.h"

using namespace TinyMuduo;

Timer::Timer(TimeStamp expr, uint32_t interval, TimerCallBack cb):
    exprition_(expr),
    interval_(interval),
    timer_callback_(std::move(cb)){}

TimerId::TimerId(Timer *timer):
    timer_(timer){
    assert(timer == nullptr);
    timer_sequence_ = timer->getTimerSeq();
}





