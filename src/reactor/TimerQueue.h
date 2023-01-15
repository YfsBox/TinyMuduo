//
// Created by 杨丰硕 on 2023/1/15.
//

#ifndef TINYMUDUO_TIMERQUEUE_H
#define TINYMUDUO_TIMERQUEUE_H

#include <atomic>
#include <functional>
#include <memory>
#include <vector>
#include <set>
#include <map>
#include "Channel.h"
#include "../base/Timestamp.h"

namespace TinyMuduo {
    class EventLoop;

    class Timer {
    public:

        using TimerCallBack = std::function<void()>;

        Timer(TimeStamp expr, uint32_t interval, TimerCallBack cb);

        ~Timer() = default;

        TimeStamp getExprition() const {
            return exprition_;
        }

        uint32_t getInterval() const {
            return interval_;
        }

        bool isRepeat() const {
            return is_repeat_;
        }

        void run() {
            timer_callback_();
        }

        uint32_t getTimerSeq() const {
            return timer_sequence_;
        }

    private:
        static int getNextNumber() {
            return created_timer_number_++;
        }

        TimeStamp exprition_;
        uint32_t interval_;
        bool is_repeat_;
        TimerCallBack timer_callback_;
        uint32_t timer_sequence_;
        static std::atomic_int created_timer_number_;
    };

    class TimerId {
    public:
        explicit TimerId(Timer *timer);

        ~TimerId() = default;

        Timer* getTimer() const {
            return timer_;
        }

        uint32_t getTimerSeq() const {
            return timer_sequence_;
        }

    private:
        Timer *timer_;
        uint32_t timer_sequence_;
    };

    class TimerQueue {
    public:
        // using TimerEntry = std::pair<TimeStamp, >;
        using TimerPtr = std::unique_ptr<Timer>;
        explicit TimerQueue(EventLoop *loop);

        ~TimerQueue();

        TimerId addTimer(TimeStamp expr, uint32_t interval, Timer::TimerCallBack cb);

        void cancelTimer(TimerId timerId);


    private:
        using TimerEntry = std::pair<Timer* , uint32_t>;
        // using SortedEntry = std::pair<TimeStamp, TimerPtr>;

        using TimerList = std::map<Timer*, uint32_t>;
        using SortedList = std::map<TimeStamp, TimerPtr>;

        void readHandle();      // 用来给channel绑定的read回调函数

        void addTimerInLoop(Timer *timer);

        void cancelTimerInLoop(TimerId timerId);

        void readTimerFd() const;

        std::vector<TimerPtr> getExprTimers(TimeStamp now);

        EventLoop *loop_;
        const int timer_fd_;
        Channel timer_channel_;

        TimerList timer_list_;
        SortedList sorted_list_;

    };
}

#endif //TINYMUDUO_TIMERQUEUE_H
