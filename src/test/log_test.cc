//
// Created by 杨丰硕 on 2023/1/10.
//
#include <gtest/gtest.h>
#include "../base/Utils.h"
#include "../base/Timestamp.h"
#include "../logger/Logger.h"
#include "../logger/LoggerStream.h"
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
            TimeStamp::getNowTimeStamp().getTimeFormatString() << '\n';
        }
    }
    printf("wait a little......\n");
    auto wait_thread = []() {
         std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    };

    AsyncLogging::getInstance().stop();     // 停止
}


int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

