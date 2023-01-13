//
// Created by 杨丰硕 on 2023/1/13.
//

#ifndef TINYMUDUO_LOOPTHREADPOOL_H
#define TINYMUDUO_LOOPTHREADPOOL_H

#include <iostream>
#include <vector>
#include <memory>

namespace TinyMuduo {
    class EventLoop;

    class EventLoopThread;

    class LoopThreadPool {
    public:
        using EventLoopThreadPtr = std::unique_ptr<EventLoopThread>;
        using EventLoopPtr = std::unique_ptr<EventLoop>;

        LoopThreadPool(EventLoop *ownerloop, const std::string name);

        ~LoopThreadPool();

        void setThreadSize(uint32_t threadsize) {
            thread_size_ = threadsize;
        }

        uint32_t getThreadSize() const {
            return thread_size_;
        }

        EventLoop *getNextLoop();

        void start();

        void stop();

    private:
        bool is_started_;

        EventLoop *owner_loop_;
        uint32_t next_loop_num_;

        std::string pool_name_;
        size_t thread_size_;

        std::vector<EventLoopThreadPtr> loop_threads_;
        std::vector<EventLoopPtr> sub_loops_;

    };
}

#endif //TINYMUDUO_LOOPTHREADPOOL_H
