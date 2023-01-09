//
// Created by 杨丰硕 on 2023/1/9.
//
#ifndef TINYMUDUO_COUNTDOWNLATCH_H
#define TINYMUDUO_COUNTDOWNLATCH_H

#include <mutex>
#include <condition_variable>

namespace TinyMuduo {

    class CountDownLatch {
    public:

        explicit CountDownLatch(int count);

        ~CountDownLatch() = default;

        CountDownLatch(const CountDownLatch &countDownLatch) = delete;

        CountDownLatch& operator=(CountDownLatch &countDownLatch) = delete;

        int getCount();

        void down();

        void wait();

    private:
        int count_;
        std::mutex lock_;
        std::condition_variable condition_;
    };

}
#endif //TINYMUDUO_COUNTDOWNLATCH_H
