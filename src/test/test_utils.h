//
// Created by 杨丰硕 on 2023/1/11.
//
#ifndef TINYMUDUO_TEST_UTILS_H
#define TINYMUDUO_TEST_UTILS_H

#include "../base/Thread.h"

using namespace TinyMuduo;

class WaitThread {
public:

    explicit WaitThread(uint32_t waitseconds);

    ~WaitThread() = default;

    void start() {
        thread_.startThread();
    }

    void join() {
        thread_.joinThread();
    }

    uint32_t getWaitTime() const {
        return wait_seconds_;
    }

private:

    void waitFunc() {
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_seconds_ * 1000));
    }

    uint32_t wait_seconds_;
    Thread thread_;
};



#endif //TINYMUDUO_TEST_UTILS_H
