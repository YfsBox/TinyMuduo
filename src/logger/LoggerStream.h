//
// Created by 杨丰硕 on 2023/1/10.
//
#ifndef TINYMUDUO_LOGGERSTREAM_H
#define TINYMUDUO_LOGGERSTREAM_H

#include <string.h>
#include "../base/Utils.h"

namespace TinyMuduo {

    static const int SmallBufferSize = 4000;
    static const int BigBufferSize = 4000 * 1000;

    template<int SIZE>
    class FixedBuffer {
    public:

        FixedBuffer():curr_(buffer_) {
            reset();        // 清空缓冲区内容
        }
        ~FixedBuffer() = default;

        void append(const char *content, size_t len) {
            // 需要考虑的是可用的条件
            if (getAvail() >= len) {
                memcpy(curr_, content, len);
                curr_ += len;
            }
        }

        const char *begin() const {
            return buffer_;
        }

        const char *getData() const {
            return buffer_;
        }

        const char *end() const {
            return buffer_ + sizeof(buffer_);
        }

        const char *getCurr() const {
            return curr_;
        }

        void reset() {
            memset(buffer_, 0, sizeof(buffer_));
        }

        size_t getAvail() const {
            return end() - getCurr();
        }

        std::string toString() const {
            return buffer_;
        }

    private:
        char buffer_[SIZE];
        char *curr_;
    };


    class LoggerStream: TinyMuduo::Utils::noncopyable {
    public:

        LoggerStream& operator<<(const std::string &str) {
            append(str.c_str(), str.length());
            return *this;
        }

        LoggerStream& operator<<(const char* str) {
            if (str == nullptr) {
                append("(nullptr)", 9);
            } else {
                append(str, strlen(str));
            }
            return *this;
        }

        LoggerStream& operator<<(int);          // 需要将int、uint转化成字符串

        LoggerStream& operator<<(uint);

        LoggerStream& operator<<(int64_t);

        LoggerStream& operator<<(uint64_t);

        LoggerStream& operator<<(int16_t);

        LoggerStream& operator<<(uint16_t);

        LoggerStream& operator<<(char ch) {
            append(&ch, 1);
            return *this;
        }

        LoggerStream& operator<<(bool b) {
            append(b ? "1":"0", 1);
            return *this;
        }

        LoggerStream& operator<<(double);

        LoggerStream& operator<<(float);

        size_t getBufferLen() const {
            return buffer_.getCurr() - buffer_.begin();
        }

        const char *getBuffer() const {
            return buffer_.begin();
        }

    private:

        template<class InterType>
        void appendForInter(InterType val);

        void append(const char *content, size_t len) {
            buffer_.append(content, len);
        }

        FixedBuffer<SmallBufferSize> buffer_;
    };
}

#endif //TINYMUDUO_LOGGERSTREAM_H
