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

        explicit TimeStamp(uint64_t micro_seconds);

        ~TimeStamp() = default;

        static const uint64_t MillSecondsPerSecond = 1000;

        static const uint64_t MicroSecondsPerSecond = 1000 * 1000;

        static const uint64_t MicroSecondsPerMillSecond = 1000;

        static TimeStamp getNowTimeStamp();

        std::string getTimeString() const;

        std::string getTimeFormatString() const;

        uint64_t getMillSeconds() const {
            return micro_seconds_ / MicroSecondsPerMillSecond;
        }

        uint64_t getMicroSeconds() const {
            return micro_seconds_;
        }

        void setMillSeconds(uint64_t mill_second) {
            micro_seconds_ = mill_second * MicroSecondsPerMillSecond;
        }

        void setMicroSeconds(uint64_t micro_second) {
            micro_seconds_ = micro_second;
        }

    private:
        uint64_t micro_seconds_;
    };

    inline bool operator<(const TimeStamp &lhs, const TimeStamp &rhs) {
        return lhs.getMicroSeconds() < rhs.getMicroSeconds();
    }
}


#endif //TINYMUDUO_TIMESTAMP_H
