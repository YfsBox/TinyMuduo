//
// Created by 杨丰硕 on 2023/1/15.
//

#ifndef TINYMUDUO_TIMERQUEUE_H
#define TINYMUDUO_TIMERQUEUE_H

#include <atomic>
#include <functional>
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
    private:
        EventLoop *loop;
        const int timer_fd_;
        Channel timer_channel_;

    };
}

#endif //TINYMUDUO_TIMERQUEUE_H
