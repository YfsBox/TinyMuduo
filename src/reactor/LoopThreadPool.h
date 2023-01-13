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
        using EventLoopPtr = EventLoop*;

        LoopThreadPool(EventLoop *ownerloop, const std::string name);

        ~LoopThreadPool();

        void setThreadSize(uint32_t threadsize) {
            thread_size_ = threadsize;
        }

        uint32_t getThreadSize() const {
            return thread_size_;
        }

        uint32_t getCurrNextNumber() const {
            return next_loop_num_;
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
        std::vector<EventLoopPtr> sub_loops_;       // 如果这里也用unique_ptr的话，会导致
// 析构函数会delete已经无效了的栈上对象，造成内存错误
    };
}

#endif //TINYMUDUO_LOOPTHREADPOOL_H
