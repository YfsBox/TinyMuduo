//
// Created by 杨丰硕 on 2023/1/8.
//
#include <random>
#include <gtest/gtest.h>
#include "../base/Thread.h"
#include "../base/Threadpool.h"

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

TEST(THREADPOOL_TEST, BASIC_THREADPOOL_TEST) {
    // 首先应该生成随机数,为每个线程分配随机的sleep时间
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(-500, 500);
    auto rnd = std::bind(dist, mt);
    // 计算一个变量,用于检测线程池的结果,并且定义线程池中的计算函数
    std::atomic<int> add_cnt = 0;
    // 定义线程池，并启动
    TinyMuduo::ThreadPool pool1(1024, 192, "pool1");

    ASSERT_EQ(pool1.getName(), "pool1");
    ASSERT_EQ(pool1.getQueueSize(), 0);
    ASSERT_EQ(pool1.isRunning(), false);

    pool1.start();
    printf("The pool1 start\n");

    // 加入240个计算任务
    size_t task_num = 2048;
    for (size_t i = 0; i < task_num; ++i) {
        pool1.pushTask([&]() {
            // printf("run task %zu", i);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 + rnd()));
            add_cnt++;
        });     // 加入计算任务
    }
    printf("The add cnt is %d\n", int(add_cnt));
    printf("push the tasks ever\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    // 关闭掉线程池
    printf("The add cnt is %d\n", int(add_cnt));

    printf("The add cnt is %d\n", int(add_cnt));

    pool1.stop();

    printf("The add cnt is %d\n", int(add_cnt));
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

