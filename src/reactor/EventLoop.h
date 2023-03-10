//
// Created by 杨丰硕 on 2023/1/8.
//

#ifndef TINYMUDUO_EVENTLOOP_H
#define TINYMUDUO_EVENTLOOP_H

#include <functional>
#include <vector>
#include <list>
#include <deque>
#include <memory>
#include <mutex>
#include <atomic>
#include "../base/Timestamp.h"
#include "../base/Thread.h"

namespace TinyMuduo {

    class Epoller;

    class Channel;

    class TimerQueue;

    class TimerId;

    class EventLoop {
    public:
        using ChannelVector = std::vector<Channel*>;
        using QueuedFunctor = std::function<void()>;
        using TimerTask = std::function<void()>;

        static const int defaultTimeoutMs = 10000;

        EventLoop();

        ~EventLoop();

        void updateChannel(Channel *channel);

        void removeChannel(Channel *channel);

        void loop(int timeout);

        void quit(); // 通常会被外部调用,用来关闭该loop

        void wakeup();

        void wakeupAndQuit();

        bool isOuterThread() const {
            return CurrThreadSpace::getCurrThreadId() != thread_id_;        // 如果返回true,那就是外部调用的
        }

        void assertInThisThread();

        void doQueuedFunctors();

        size_t getQueueSize();

        void runInLoop(QueuedFunctor func);

        TimerId runTimertaskAt(TimeStamp timestamp, TimerTask task);

        TimerId runTimertaskAfter(uint64_t delay, TimerTask task);

    private:

        int createWakeupFd();

        void wakeupHandle();

        std::atomic_bool is_looping_;
        std::atomic_bool is_quit_;
        const pid_t thread_id_;
        std::mutex mutex_;
        std::unique_ptr<Epoller> epoller_;  // 一个EventLoop是一个Epoller的所有者
        std::unique_ptr<TimerQueue> timer_queue_;

        int wakeup_fd_;
        std::unique_ptr<Channel> wakeup_channel_;

        ChannelVector active_channels_;     // 用于存放从epoller中所读取的channels
        std::deque<QueuedFunctor> queued_list_;
    };
}

#endif //TINYMUDUO_EVENTLOOP_H
