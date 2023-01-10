//
// Created by 杨丰硕 on 2023/1/10.
//
#include <gtest/gtest.h>
#include "../base/Timestamp.h"
#include "../logger/LoggerStream.h"

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


int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

