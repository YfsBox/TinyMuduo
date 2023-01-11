//
// Created by 杨丰硕 on 2023/1/10.
//
#include <gtest/gtest.h>
#include "test_utils.h"
#include "../base/Threadpool.h"
#include "../base/Utils.h"
#include "../logger/Logger.h"
#include "../logger/AsyncLogging.h"

using namespace TinyMuduo;

TEST(LOG_TEST, LOGGERSTREAM_TEST) {
    // 测试各种类型
    // int
    LoggerStream loggerStream;
    for (size_t i = 0; i < 40; ++i) {
        loggerStream << TimeStamp::getNowTimeStamp().getMillSeconds() << '\n';
        loggerStream << TimeStamp::getNowTimeStamp().getTimeFormatString() << '\n';
    }

    loggerStream << "1234jdjsidjisjdisdisnxjyuwavvajskd00123nnjandjsbcbcudyabbaj1919amm\n";

    printf("The LoggerStream Buffer len is %zu\n", loggerStream.getBufferLen());
    // 打印结果观察一下:
    printf("The buffer is:\n%s", loggerStream.getBuffer());
}

TEST(LOG_TEST, LOGGER_TEST) {

    for (size_t i = 0; i < 50; ++i) {
        LOG_DEBUG << TimeStamp::getNowTimeStamp().getTimeFormatString() << '\n';
        LOG_DEBUG << TimeStamp::getNowTimeStamp().getMillSeconds() << '\n';
    }

    LOG_INFO << "dsjdjshdjshadhhwudhubjbcbjhshdkajldasldkslnmcn9128923811ppppds/;..ds";
}

TEST(LOG_TEST, ASYNCLOGGING_TEST) {

    AsyncLogging::getInstance().init(3, "test_log");
    AsyncLogging::getInstance().start();

    ASSERT_TRUE(AsyncLogging::getInstance().isRunning());
    {
        Utils::CalTimeUtil caltime;
        for (size_t i = 0; i < 500; ++i) {
            LOG_INFO << "hello,world asdfghjklzxcvbnmqwertyuiop" <<
            TimeStamp::getNowTimeStamp().getTimeFormatString() << "the cnt is " << i <<'\n';
        }
    }
    printf("wait a little......\n");
    auto wait_thread_func = []() {
         std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    };

    Thread wait_thread(wait_thread_func);
    wait_thread.startThread();
    wait_thread.joinThread();

    AsyncLogging::getInstance().stop();     // 停止

}

TEST(LOG_TEST, ASYNCLOGGING_MUlTI_TEST) {       // 模拟多个线程写日志的场景进行测试
    // 启动日志系统
    AsyncLogging::getInstance().init(3, "multi_test_log");
    AsyncLogging::getInstance().start();
    ASSERT_TRUE(AsyncLogging::getInstance().isRunning());
    // 启动一个线程池
    ThreadPool pool(192, 192);
    pool.start();
    // 加入输出日志的任务,目标1000条日志
    for (size_t i = 0; i < 1000; ++i) {
        pool.pushTask([i](){
            size_t wait_time = 200 + (static_cast<size_t>(std::rand()) % 500);
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
            LOG_DEBUG << "hello,world asdfghjklzxcvbnmqwertyuiop" <<
            " the log id is " << i << " and wait " << wait_time << "mills\n";
        });
    }
    // 模拟其他计算操作
    std::atomic_int wait_cnt = 1000;
    printf("push many cnt sub......\n");
    for (size_t i = 0; i < 1000; ++i) {
        pool.pushTask([&wait_cnt]() {
            size_t wait_time = 200 + (static_cast<size_t>(std::rand()) % 500);
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
            wait_cnt--;
        });
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    printf("stop pool and logger system and check wait cnt\n");
    ASSERT_EQ(wait_cnt, 0);
    // 关闭线程池和日志系统
    // pool.stop();
    AsyncLogging::getInstance().stop();
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

