//
// Created by 杨丰硕 on 2023/1/9.
//
#include "CountDownLatch.h"

using namespace TinyMuduo;

CountDownLatch::CountDownLatch(int count):
    count_(count),
    lock_(),
    condition_(){

}

void CountDownLatch::wait() {
    std::unique_lock<std::mutex> guard(lock_);
    while (count_ > 0) {
        condition_.wait(guard);
    }
}

void CountDownLatch::down() {
    std::lock_guard<std::mutex> guard(lock_);
    count_--;
    if (count_ == 0) {
        condition_.notify_all();
    }
}

int CountDownLatch::getCount() {
    std::lock_guard<std::mutex> guard(lock_);
    return count_;
}

