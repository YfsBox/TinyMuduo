//
// Created by 杨丰硕 on 2023/1/8.
//

#ifndef TINYMUDUO_UTILS_H
#define TINYMUDUO_UTILS_H

#include <iostream>
#include <random>
#include "../base/Timestamp.h"

namespace TinyMuduo::Utils {
    /*
    class DebugUtil {
    public:
        explicit DebugUtil(const std::string &msg);

        ~DebugUtil();

        DebugUtil(const DebugUtil &debug) = delete;

        DebugUtil &operator=(const DebugUtil &debug) = delete;

    private:
        std::string msg_;
    };

    DebugUtil::DebugUtil(const std::string &msg) : msg_(msg) {
        std::cout << "Debug begin: " << msg_ << '\n';
    }

    DebugUtil::~DebugUtil() {
        std::cout << "Debug end: " << msg_ << '\n';
    }*/

    class noncopyable {
    public:
        noncopyable(const noncopyable&) = delete;
        void operator=(const noncopyable&) = delete;

    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    };

    class CalTimeUtil {     // 利用RAII进行计时, 并输出
    public:
        explicit CalTimeUtil():start_time_(TimeStamp::getNowTimeStamp()) {}

        ~CalTimeUtil() {
            auto end_time_mills = TimeStamp::getNowTimeStamp().getMillSeconds();
            std::cout << "The time cost is " << (end_time_mills - start_time_.getMillSeconds()) /
            TimeStamp::MillSecondsPerSecond << '\n';
        }

        CalTimeUtil(const CalTimeUtil&) = delete;
        void operator=(const CalTimeUtil&) = delete;

    private:
        TimeStamp start_time_;
    };


}






#endif //TINYMUDUO_UTILS_H
