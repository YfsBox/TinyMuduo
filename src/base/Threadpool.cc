//
// Created by 杨丰硕 on 2023/1/9.
//
#include <assert.h>
#include "Thread.h"
#include "Threadpool.h"

using namespace TinyMuduo;

const std::string ThreadPool::DEFAULT_POOL_NAME = "ThreadPool";

ThreadPool::ThreadPool(size_t psize, size_t qsize, const std::string &name):
    is_running_(false),
    pool_size_(psize),
    max_queue_size_(qsize),
    name_(name) {
    pool_threads_.reserve(pool_size_);
}

ThreadPool::~ThreadPool() {     // 关闭所有工作单元
    if (is_running_) {
        stop();
    }
}

void ThreadPool::start() {
    // 创建一个个的thread
    is_running_ = true;
    for (size_t i = 0; i < pool_size_; ++i) {
        std::string default_name = DEFAULT_POOL_NAME + "_" + Thread::getDefaultName();
        std::function<void()> dotask_func = std::bind(&ThreadPool::doTaskInThread, this);
        pool_threads_.push_back(std::make_unique<Thread>(std::move(dotask_func), default_name));
        pool_threads_[i]->startThread(); // 开启thread
    }
}

void ThreadPool::stop() {
    is_running_ = false;
    not_full_condition_.notify_all();
    not_empty_condition_.notify_all();
    for (auto &thread : pool_threads_) {
        thread->joinThread();
    }
}

void ThreadPool::pushTask(Task task) {      // 将Task加入到队列
    if (!is_running_) {     // 没有运行的话,就直接返回
        return;
    }

    if (pool_threads_.empty()) {
        task();
    } else {
        std::unique_lock<std::mutex> lockGuard(lock_);
        while (isQueueFull()) {
            not_full_condition_.wait(lockGuard);
        }
        // 将任务加入到queue中
        if (!is_running_) {     // 如果已经不再运行了就直接返回
            return;
        }
        tasks_queue_.push_back(std::move(task));
        not_empty_condition_.notify_one(); // 随便唤醒一个
        // printf("push Task ok\n");
    }
    // 需要唤醒not_empty
}// 在while循环后面之所以加了一个is_running的判断，是因为从条件变量中唤醒除了popTask，还有stop的原因

ThreadPool::Task ThreadPool::popTask() {    // 从队列里取出一个task,提供给线程运行
    Task task;
    std::unique_lock<std::mutex> lockGuard(lock_);
    while (isQueueEmpty()) {    // 如果是empty,就等待not empty
        not_empty_condition_.wait(lockGuard);
    }
    if (!is_running_) {
        return task;
    }
    task = tasks_queue_.front();
    tasks_queue_.pop_front();   // FIFO

    not_full_condition_.notify_one(); // 需要唤醒not_full
    return task;
}

void ThreadPool::doTaskInThread() {
    // thread 通过读取queue的方式从中获取任务并且运行
    while (is_running_) {
        Task task = popTask();      // 取出一个任务
        if (!is_running_) {
            break;
        }
        if (task) {
            task();
        }
    }
}


// 需要了解一些一下unique_lock和lock_guard的区别







