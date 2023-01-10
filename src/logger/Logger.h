//
// Created by 杨丰硕 on 2023/1/8.
//

#ifndef TINYMUDUO_LOGGER_H
#define TINYMUDUO_LOGGER_H

#include <vector>
#include <functional>
#include "../base/Timestamp.h"
#include "../base/Utils.h"
#include "LoggerStream.h"

namespace TinyMuduo {

    class Logger: TinyMuduo::Utils::noncopyable {
    public:

        using outputFunc = std::function<void(const char *, size_t)>;

        enum LogLevel{
            INFO = 0,
            ERROR,
            DEBUG,
            FATAL,
        };

        static std::vector<std::string> LevelMapVec;

        static LogLevel LoggerLevel;

        static outputFunc LoggerOutput;

        Logger(const char *filename, unsigned int lineno, LogLevel level, const char *func);

        ~Logger();

        static LogLevel getLoggerLevel() {
            return LoggerLevel;
        }

        static void setLoggerLevel(LogLevel level) {
            LoggerLevel = level;
        }

        static void setLoggerOutput(outputFunc ofunc);

        LoggerStream& getStream() {
            return stream_;
        }

    private:

        std::string getFileName(const char *name);
        unsigned int line_number_;
        LogLevel level_;
        LoggerStream stream_;
        TimeStamp time_stamp_;      // 时间戳
    };


#define LOG_INFO    \
TinyMuduo::Logger(__FILE__, __LINE__, TinyMuduo::Logger::LogLevel::INFO, __FUNCTION__).getStream()

#define LOG_ERROR   \
TinyMuduo::Logger(__FILE__, __LINE__, TinyMuduo::Logger::LogLevel::ERROR, __FUNCTION__).getStream()

#define LOG_DEBUG   \
TinyMuduo::Logger(__FILE__, __LINE__, TinyMuduo::Logger::LogLevel::DEBUG, __FUNCTION__).getStream()

#define LOG_FATAL   \
TinyMuduo::Logger(__FILE__, __LINE__, TinyMuduo::Logger::LogLevel::FATAL, __FUNCTION__).getStream()
// 利用匿名对象

    // void defaultOutput(const char *buf, size_t len);

}

#endif //TINYMUDUO_LOGGER_H
