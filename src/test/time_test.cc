//
// Created by 杨丰硕 on 2023/1/8.
//
#include "../base/Timestamp.h"
#include <gtest/gtest.h>

TEST(TIMESTAMP_TEST, SIMPLE_TEST) {
    auto curr_time1 = TinyMuduo::TimeStamp::getNowTimeStamp();
    std::cout << curr_time1.getTimeFormatString() << '\n';
    sleep(3);
    auto curr_time2 = TinyMuduo::TimeStamp::getNowTimeStamp();
    std::cout << curr_time2.getTimeFormatString() << '\n';

    for (size_t i = 0; i < 5; ++i) {
        auto curr_time_tmp = TinyMuduo::TimeStamp::getNowTimeStamp();
        std::cout << curr_time_tmp.getTimeFormatString() << '\n';
    }
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}




