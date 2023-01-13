//
// Created by 杨丰硕 on 2023/1/13.
//
#include <gtest/gtest.h>
#include "../logger/Logger.h"
#include "../reactor/EventLoop.h"
#include "../reactor/EventLoopThread.h"
#include "../reactor/LoopThreadPool.h"

using namespace TinyMuduo;

TEST(EVENTLOOPTHREAD_TEST, BASIC_EVENTLOOPTHREAD_TEST) {


}

TEST(EVENTLOOPTHREAD_TEST, LOOPTHREADPOOL_TEST) {
    // 首先创建owner loop
    auto owner_loop= std::make_unique<EventLoop>();
    auto raw_owner_loop = owner_loop.get();
    LoopThreadPool pool(raw_owner_loop, "loop thread pool");
    ASSERT_EQ(pool.getThreadSize(), 0);
    ASSERT_EQ(pool.getNextLoop(), raw_owner_loop);
    // 以thread size为0的方式启动
    pool.start();
    ASSERT_EQ(pool.getThreadSize(), 0);
    ASSERT_EQ(pool.getNextLoop(), raw_owner_loop);
    pool.stop();
    // 设置大小为20的pool
    pool.setThreadSize(20);
    ASSERT_EQ(pool.getThreadSize(), 20);
    pool.start();
    std::atomic_int check_cnt = 0;

    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 20; ++j) {
            ASSERT_EQ(pool.getCurrNextNumber(), j);
            auto tmploop = pool.getNextLoop();
            ASSERT_NE(tmploop, raw_owner_loop);     // 不和owner相等
            tmploop->runInLoop([&check_cnt, tmploop](){
                LOG_DEBUG << "the subloop run the add task when cnt is" << check_cnt;
                check_cnt++;
            });
        }
    }
    LOG_DEBUG << "wait and stop......";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    pool.stop();
    ASSERT_EQ(check_cnt, 60);
}


int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}
