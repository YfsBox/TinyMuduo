//
// Created by 杨丰硕 on 2023/1/9.
//
#include "../base/Timestamp.h"
#include "../base/Thread.h"
#include "../base/Threadpool.h"
#include "../reactor/Epoller.h"
#include "../reactor/EventLoop.h"
#include "../reactor/EventLoopThread.h"
#include <gtest/gtest.h>

using namespace TinyMuduo;

TEST(EPOLLER_TEST, BASIC_EPOLLER_TEST) {
    Epoller epoller;
    ASSERT_TRUE(epoller.getEpollerFd() > 0);
    Epoller::ChannelVector channels;
    epoller.epoll(5, channels);
    printf("epoll ok after 5 seconds");
}

TEST(EVENTLOOP_TEST, BASIC_EVENTLOOP_TEST) {
    int timeout_second = 3;
    auto thread_func = [&]() {
        printf("thread_func: tid = %d\n", CurrThreadSpace::CurrThreadId);
        EventLoop loop;
        ASSERT_TRUE(CurrThreadSpace::LoopInThisThread != nullptr);
        loop.loop(timeout_second);
    };

    CurrThreadSpace::setCurrThreadId(); // 主线程设置tid
    printf("The main thread id is: %d\n", CurrThreadSpace::CurrThreadId);
    EventLoop main_loop;
    ASSERT_TRUE(CurrThreadSpace::LoopInThisThread != nullptr);      // 主线程对应的loop不为nullptr

    auto quit_func = [&]() {    // 用于退出main_loop的loop
        sleep(3);
        main_loop.wakeupAndQuit();       // 退出main_loop的loop
    };

    Thread quit_thread(quit_func);
    quit_thread.startThread();
    main_loop.loop(timeout_second);

    printf("quit main loop successfully!\n");

    {
        Thread son_thread(thread_func);
        son_thread.startThread();
        sleep(3);
    }   // 利用析构函数将子线程进行detach
    printf("the son thread has detached!\n");

}

TEST(EVENTLOOP_TEST, QUEUE_TEST) {
    std::atomic_int task_cnt = 50;
    // 开启loop thread
    {
        EventLoopThread loop_thread("loop_thread");
        auto loop = loop_thread.startAndGetEventLoop();
        ASSERT_TRUE(loop != nullptr);
        ASSERT_TRUE(loop_thread.isLooping());
        // 创建线程池，大小为10
        ThreadPool pool(10, 20);
        pool.start();
        for (size_t i = 0; i < 50; ++i) {
            // 加入50个基于runInLoop的计算任务
            pool.pushTask([&task_cnt, loop]() {
                loop->runInLoop([&task_cnt]() {
                    task_cnt--;
                });
            });
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        pool.stop();        // 等待线程池中的任务都执行完
    }
    ASSERT_EQ(task_cnt, 0);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}