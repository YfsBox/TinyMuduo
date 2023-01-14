//
// Created by 杨丰硕 on 2023/1/10.
//
#ifndef TINYMUDUO_BUFFER_H
#define TINYMUDUO_BUFFER_H

#include <iostream>
#include <vector>

namespace TinyMuduo {
    class Buffer {
    public:

        static const int PREPEND_BUFFER_SIZE = 8;

        static const int INIT_BUFFER_SIZE = 1024;

        explicit Buffer(size_t init_buffer_size = INIT_BUFFER_SIZE);

        ~Buffer();

        const char *peek() const {        // 返回read的开头对应的指针
            return begin() + read_index_;
        }

        const char *writableHeader() const {
            return begin() + write_index_;
        }

        char *writableHeader() {
            return const_cast<char*>(begin() + write_index_);   // 去除const
        }

        size_t getReadableSize() const {
            return write_index_ - read_index_;      // 用index可以防止迭代器失效的情况
        }

        size_t getWritableSize() const {
            return buffer_.size() - write_index_;
        }

        size_t getPrependSize() const {
            return read_index_;
        }

        void retrieve(size_t len) {
            if (len <= getReadableSize()) {
                read_index_ += len;
            } else {
                retrieveAll();
            }
        }

        void retrieveAll() {
            read_index_ = write_index_ = PREPEND_BUFFER_SIZE;
        }

        std::string retrieveAsString(size_t len) {
            std::string result(peek(), len);
            retrieve(len);
            return result;
        }

        void ensureWritable(size_t len) {       // 当可写空间不足时就进行扩容
            if (getWritableSize() < len) {
                extendBuffer(len);
            }
        }

        void append(const std::string &content) {           // 将一段内容写入到buffer中去,写入到可读buffer中
            append(content.c_str(), content.size());
        }

        void append(const char *content, size_t len) {
            ensureWritable(len);
            std::copy(content, content + len, writableHeader());
            write_index_ += len;
        }

        ssize_t read(int fd, int *err);

        ssize_t write(int fd, int *err);

    private:

        void extendBuffer(size_t len) {
            if (getWritableSize() + getReadableSize() - PREPEND_BUFFER_SIZE < len) {
                // 如果buffer中的所有内存碎片加起来都小于len
                buffer_.resize(write_index_ + len);
            } else {
                // 移动到开头
                size_t readable_size = getReadableSize();
                std::copy(begin() + read_index_, begin() + write_index_,
                          begin() + PREPEND_BUFFER_SIZE );
                read_index_ = PREPEND_BUFFER_SIZE;
                write_index_ = read_index_ + readable_size;
            }
        }

        char *begin() {
            return const_cast<char*>(&*buffer_.begin());
        }

        const char *begin() const {
            return &*buffer_.begin();
        }

        std::vector<char> buffer_;
        size_t read_index_;
        size_t write_index_;
    };
}

#endif //TINYMUDUO_BUFFER_H
