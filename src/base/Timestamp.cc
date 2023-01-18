//
// Created by 杨丰硕 on 2023/1/8.
//
#include <ctime>
#include "Timestamp.h"

using namespace TinyMuduo;

TimeStamp::TimeStamp(uint64_t micro_seconds): micro_seconds_(micro_seconds) {
}

TimeStamp TimeStamp::getNowTimeStamp() {
    TimeStamp nowStamp;
    auto now_time_point= std::chrono::system_clock::now();
    nowStamp.setMicroSeconds(static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>
            (now_time_point.time_since_epoch()).count()));
    return nowStamp;
}

std::string TimeStamp::getTimeString() const {
    std::string time_str;
    return time_str;
}

std::string TimeStamp::getTimeFormatString() const {    // 返回格式化时间对应的字符串
    time_t seconds = static_cast<time_t>(micro_seconds_ / MicroSecondsPerSecond);
    char buf[64] = {0};

    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);
    snprintf(buf, sizeof(buf), "%4d%02d%02d-%02d:%02d:%02d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);

    return buf;
}
