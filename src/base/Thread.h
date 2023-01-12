//
// Created by 杨丰硕 on 2023/1/8.
//

#ifndef TINYMUDUO_THREAD_H
#define TINYMUDUO_THREAD_H

#include <thread>
#include <memory>
#include <atomic>
#include <functional>
#include <unistd.h>

namespace TinyMuduo {

    class EventLoop;
    class CountDownLatch;

    namespace CurrThreadSpace {

        extern thread_local pid_t CurrThreadId;

        extern thread_local EventLoop* LoopInThisThread;

        void setCurrThreadId();

        pid_t getCurrThreadId();

    }

    class Thread {
    public:
        using ThreadFunc = std::function<void()>;

        Thread(ThreadFunc func);       // 使用默认生成的name

        Thread(ThreadFunc func, const std::string &name);

        ~Thread();

        void startThread();

        void joinThread();

        std::string getThreadName() const {
            return thread_name_;
        }

        pid_t getThreadId() const {
            return thread_id_;
        }

        static uint32_t getCreatedThreadNum();

        static std::string getDefaultName();

    private:

        void setFunc(ThreadFunc func);

        bool is_join_;
        bool is_start;
        EventLoop *loop_;
        pid_t thread_id_;  // 如何通过thread获取其中的thread_id呢
        std::unique_ptr<std::thread> thread_; // 如果直接使用std::thread作为对象的话，就会在构造函数期间就开始运行了
        std::shared_ptr<CountDownLatch> countdown_latch_;
        std::string thread_name_;
        ThreadFunc main_func_;      // 该thread的主循环函数
        static std::atomic<uint32_t> created_thread_num_;

    };
}

#endif //TINYMUDUO_THREAD_H
