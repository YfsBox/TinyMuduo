//
// Created by 杨丰硕 on 2023/1/10.
//
#include "LoggerStream.h"

using namespace TinyMuduo;

LoggerStream &LoggerStream::operator<<(int val) {
    std::string str(std::to_string(val));
    append(str.c_str(), str.length());
    return *this;
}

LoggerStream &LoggerStream::operator<<(uint val) {
    *this << static_cast<int>(val);
    return *this;
}

LoggerStream &LoggerStream::operator<<(double val) {
    std::string str(std::to_string(val));
    append(str.c_str(), str.length());
    return *this;
}

LoggerStream &LoggerStream::operator<<(float val) {
    *this << static_cast<double>(val);
    return *this;
}




