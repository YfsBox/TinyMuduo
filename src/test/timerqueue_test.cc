//
// Created by 杨丰硕 on 2023/1/15.
//
#include <gtest/gtest.h>
#include "../logger/Logger.h"
#include "../base/Timestamp.h"
#include "../reactor/EventLoopThread.h"
#include "../reactor/TimerQueue.h"

using namespace TinyMuduo;

TEST(TIMERQUEUE_TEST, NOREPEATE_TEST) {
    LOG_INFO << "begin the test time start at " << TimeStamp::getNowTimeStamp().getTimeFormatString();
    EventLoopThread loop_thread("test1");
    auto loop = loop_thread.startAndGetEventLoop();

    std::atomic_int test_cnt1 = 5;
    for (size_t i = 1; i <= 5; ++i) {
        loop->runTimertaskAfter( i * TimeStamp::MicroSecondsPerSecond, [&test_cnt1](){
            test_cnt1--;
        });
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
    ASSERT_EQ(test_cnt1, 0);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}