//
// Created by 杨丰硕 on 2023/1/8.
//
#include <stdio.h>
#include "../base/Thread.h"
#include "AsyncLogging.h"
#include "Logger.h"

using namespace TinyMuduo;

std::vector<std::string> Logger::LevelMapVec = {
        "INFO",
        "ERROR",
        "DEBUG",
        "FATAL",
};

void defaultOutput(const char *buf, size_t len) {
    if (AsyncLogging::getInstance().isRunning()) {
         AsyncLogging::getInstance().append(buf, len);
    } else {
        ::fwrite(buf, 1, len, stdout);    // 写入标准输出
    }
}

Logger::outputFunc Logger::LoggerOutput(defaultOutput);

Logger::Logger(const char * filename, unsigned int lineno, LogLevel level, const char *func):
        line_number_(lineno),
        level_(level),
        stream_(),
        time_stamp_(TimeStamp::getNowTimeStamp()) {
    stream_ << "[" << time_stamp_.getTimeFormatString() << "]";
    stream_ << "[tid:" << CurrThreadSpace::getCurrThreadId() << "]";
    stream_ << "[" << getFileName(filename) << ": " << lineno;
    stream_ << " " << func << "]";
    stream_ << "[" << LevelMapVec[level_] << "]";
}

Logger::~Logger() {     // RAII真是无处不在
    // 将buffer中的内容代入到output函数中
    stream_ << "\n";
    auto buffer = stream_.getBuffer();
    LoggerOutput(buffer, stream_.getBufferLen());
}

void Logger::setLoggerOutput(outputFunc ofunc) {
    LoggerOutput = ofunc;
}

std::string Logger::getFileName(const char *name) {
    const char* slash = strrchr(name, '/');
    if (slash) {
        return slash + 1;
    }
    return name;
}
