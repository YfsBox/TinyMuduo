//
// Created by 杨丰硕 on 2023/1/8.
//
#include <gtest/gtest.h>
#include "../base/Timestamp.h"
#include "../logger/Logger.h"


TEST(TIMESTAMP_TEST, SIMPLE_TEST) {
    auto curr_time1 = TinyMuduo::TimeStamp::getNowTimeStamp();
    // std::cout << curr_time1.getTimeFormatString() << '\n';
    LOG_INFO << curr_time1.getMicroSeconds();
    sleep(3);
    auto curr_time2 = TinyMuduo::TimeStamp::getNowTimeStamp();
    LOG_INFO << curr_time2.getMicroSeconds();
    for (size_t i = 0; i < 5; ++i) {
        auto curr_time_tmp = TinyMuduo::TimeStamp::getNowTimeStamp();
        LOG_INFO << curr_time_tmp.getMicroSeconds();
    }
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}




