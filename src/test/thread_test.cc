//
// Created by 杨丰硕 on 2023/1/8.
//
#include "../base/Thread.h"
#include <gtest/gtest.h>

TEST(THREAD_TEST, BASIC_TEST) {
    std::atomic<uint32_t> num_cnt = 0;
    auto func_add = [&]() {
        num_cnt++;
    };
    auto func_sub = [&]() {
        num_cnt--;
    };
    using ThreadShptr = std::shared_ptr<TinyMuduo::Thread>;

    std::vector<ThreadShptr> add_threads;
    std::vector<ThreadShptr> sub_threads;
    for (size_t i = 0; i < 50; ++i) {

        auto tmp_add_thread = std::make_shared<TinyMuduo::Thread>(func_add);
        auto tmp_sub_thread = std::make_shared<TinyMuduo::Thread>(func_sub);

        add_threads.push_back(tmp_add_thread);
        sub_threads.push_back(tmp_sub_thread);
    }
    // 检查一下created
    ASSERT_EQ(TinyMuduo::Thread::getCreatedThreadNum(), 100);
    ASSERT_EQ(TinyMuduo::Thread::getDefaultName(), "thread_100");
    // 然后集中开启,等待一会并检查结果
    for (size_t i = 0; i < 50; ++i) {
        add_threads[i]->startThread();
    }
    sleep(1);
    ASSERT_EQ(num_cnt, 50);
    for (size_t i = 0; i < 50; ++i) {
        sub_threads[i]->startThread();
    }
    sleep(1);
    ASSERT_EQ(num_cnt, 0);

}

TEST(THREAD_TEST, THREADID_TEST) {
    auto sleep_func = []() {
        while (1);
    };
    using ThreadShptr = std::shared_ptr<TinyMuduo::Thread>;
    {
        std::vector<ThreadShptr> threads;

        for (size_t i = 0; i < 100; ++i) {
            auto thread_ptr = std::make_shared<TinyMuduo::Thread>(sleep_func);
            threads.emplace_back(thread_ptr);
        }
        // 逐个开启
        for (auto &thread: threads) {
            thread->startThread();
        }
        sleep(1);
        for (const auto &thread: threads) {
            // 展示thread id
            std::cout << thread->getThreadName() << "thread id is " << thread->getThreadId() << '\n';
        }
        /*
         * // join these thread, 由于是死循环所以join等不到
        for (auto &thread: threads) {
            thread->joinThread();   // 逐个关闭
        }*/
    }

}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

