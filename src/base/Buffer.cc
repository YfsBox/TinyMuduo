//
// Created by 杨丰硕 on 2023/1/10.
//
#include <sys/uio.h>
#include <unistd.h>
#include "Buffer.h"

using namespace TinyMuduo;

const int Buffer::PREPEND_BUFFER_SIZE = 8;

const int Buffer::INIT_BUFFER_SIZE = 1024;

const char Buffer::CRLF[] = "\r\n";

Buffer::Buffer(size_t init_buffer_size):
    read_index_(PREPEND_BUFFER_SIZE),
    write_index_(PREPEND_BUFFER_SIZE){
    buffer_.resize(PREPEND_BUFFER_SIZE + init_buffer_size);
}

// 这里利用iovec可以读取到非连续的多个buffer中的特点，处理了readv到buffer中需要扩容的情况
// read的话，由于考虑了缓冲区拓展问题，因此需要借助readv处理非连续buffer的能力进行读取
ssize_t Buffer::read(int fd, int *err) {
    size_t extra_max_len = 65536;
    char extra_buffer[extra_max_len];
    iovec iovecs[2];
    iovecs[0].iov_base = writableHeader();
    size_t writable = getWritableSize();
    iovecs[0].iov_len = writable;
    iovecs[1].iov_base = extra_buffer;
    iovecs[1].iov_len = extra_max_len;

    size_t iovec_cnt = (writable < sizeof(extra_buffer)) ? 2 : 1;
    ssize_t ret = ::readv(fd, iovecs, iovec_cnt);
    if (ret < 0) {
        *err = errno;
    } else if (static_cast<size_t>(ret) < writable) {  // 说明没有数据写到extra上
        write_index_ += ret;        // readv对应的是将内容写入到buffer中，因此移动的是write_index
    } else {
        write_index_ = buffer_.size(); // 对应了readv中读取到buffer上的数据
        append(extra_buffer, ret - writable);
    }

    return ret;
}

ssize_t Buffer::write(int fd, int *err) {   // 写fd意味着从Buffer的read区中读出
    int ret = ::write(fd, peek(), getReadableSize());
    if (ret < 0) {      // write对应的是读出buffer
        *err = errno;
    }
    return ret;
}
// 写的话直接调用write进行写就好了