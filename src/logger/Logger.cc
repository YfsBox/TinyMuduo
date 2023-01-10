//
// Created by 杨丰硕 on 2023/1/8.
//
#include <stdio.h>
#include "Logger.h"

using namespace TinyMuduo;

std::vector<std::string> Logger::LevelMapVec = {
        "INFO",
        "ERROR",
        "DEBUG",
        "FATAL",
};

void defaultOutput(const char *buf, size_t len) {
    ::fwrite(buf, 1, len, stdout);    // 写入标准输出
}

Logger::outputFunc Logger::LoggerOutput(defaultOutput);

Logger::Logger(unsigned int lineno, LogLevel level, const char *func):
        line_number_(lineno),
        level_(level),
        stream_(),
        time_stamp_(TimeStamp::getNowTimeStamp()) {
    stream_ << "[" << LevelMapVec[level_] << "] ";
    stream_ << time_stamp_.getTimeFormatString();
    stream_ << " LINE_NO: " << lineno;
    stream_ << " " << func;
}

Logger::~Logger() {
    // 将buffer中的内容代入到output函数中
    auto buffer = stream_.getBuffer();
    LoggerOutput(buffer, stream_.getBufferLen());
}

void Logger::setLoggerOutput(outputFunc ofunc) {
    LoggerOutput = ofunc;
}
