//
// Created by 杨丰硕 on 2023/1/12.
//

#ifndef TINYMUDUO_EVENTLOOPTHREAD_H
#define TINYMUDUO_EVENTLOOPTHREAD_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <memory>
#include "../base/Thread.h"

namespace TinyMuduo {
    class EventLoop;
    class EventLoopThread {  // 这里的thread我只打算作为需要epoller监听事件的线程
    public:

        EventLoopThread(const std::string &name);

        ~EventLoopThread();

        EventLoop *startAndGetEventLoop();

        bool isLooping() const {
            return is_looping_;
        }

    private:

        void threadLoopFunc();

        std::atomic_bool is_looping_;
        EventLoop *loop_;           // 这里的loop采用栈上对象的方式创建
        std::unique_ptr<Thread> thread_;
        std::mutex mutex_;
        std::condition_variable condition_;
    };

}


#endif //TINYMUDUO_EVENTLOOPTHREAD_H
