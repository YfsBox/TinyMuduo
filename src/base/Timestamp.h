//
// Created by 杨丰硕 on 2023/1/8.
//
#ifndef TINYMUDUO_TIMESTAMP_H
#define TINYMUDUO_TIMESTAMP_H

#include <chrono>
#include <iostream>

namespace TinyMuduo {
    class TimeStamp {
    public:
        TimeStamp() = default;

        explicit TimeStamp(uint64_t mill_seconds);

        ~TimeStamp() = default;

        static const uint64_t MillSecondsPerSecond = 1000;

        static TimeStamp getNowTimeStamp();

        std::string getTimeString() const;

        std::string getTimeFormatString() const;

        uint64_t getMillSeconds() const {
            return mill_seconds_;
        }

        void setMillSeconds(uint64_t mill_second) {
            mill_seconds_ = mill_second;
        }

    private:
        uint64_t mill_seconds_;
    };
}


#endif //TINYMUDUO_TIMESTAMP_H
