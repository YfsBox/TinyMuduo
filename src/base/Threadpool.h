//
// Created by 杨丰硕 on 2023/1/9.
//
#ifndef TINYMUDUO_THREADPOOL_H
#define TINYMUDUO_THREADPOOL_H

#include <iostream>
#include <mutex>
#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <functional>
#include <condition_variable>

namespace TinyMuduo {
    class Thread;
    class ThreadPool {
    public:

        using Task = std::function<void()>;         // 任务单元

        static const size_t DEFAULT_POOL_SIZE = 48;

        static const size_t DEFAULT_QUEUE_SIZE = 48;

        static const std::string DEFAULT_POOL_NAME;

        ThreadPool(size_t psize = DEFAULT_POOL_SIZE,
                   size_t qsize = DEFAULT_QUEUE_SIZE,
                   const std::string &name = DEFAULT_POOL_NAME);

        ~ThreadPool();

        void start();

        void stop();

        void pushTask(Task task);

        Task popTask();

        std::string getName() const {
            return name_;
        }

        size_t getPoolSize() const {
            return pool_size_;
        }

        bool isRunning() const {
            return is_running_;
        }

        size_t getQueueSize() {     // 需要用锁保护queue
            std::lock_guard<std::mutex> lockGuard(lock_);
            return tasks_queue_.size();
        }

        // using PoolThreads = std::vector<>;
    private:

        void doTaskInThread();

        bool isQueueFull() const {
            return tasks_queue_.size() >= max_queue_size_;
        }

        bool isQueueEmpty() const {
            return tasks_queue_.empty();
        }

        std::atomic_bool is_running_;
        size_t pool_size_;
        size_t max_queue_size_;
        std::string name_;
        std::mutex lock_;
        std::condition_variable not_empty_condition_;
        std::condition_variable not_full_condition_;
        std::vector<std::unique_ptr<Thread>> pool_threads_;
        std::deque<Task> tasks_queue_;      // 关于队列的操作,需要用锁来保证线程安全
    };
}

#endif //TINYMUDUO_THREADPOOL_H
