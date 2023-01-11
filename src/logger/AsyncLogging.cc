//
// Created by 杨丰硕 on 2023/1/10.
//
#include <fstream>
#include "../base/Timestamp.h"
#include "AsyncLogging.h"

using namespace TinyMuduo;

const std::string AsyncLogging::DEFAULT_LOGTHREAD_NAME = "Log Backend Thread";


AsyncLogging::AsyncLogging(uint32_t interval, const std::string &filename):
    is_running_(false),
    flush_interval_(interval),
    logfile_name_(filename),
    log_thread_(std::bind(&AsyncLogging::logThreadFunc, this),
                DEFAULT_LOGTHREAD_NAME) {
    curr_buffer_ = std::make_unique<Buffer>();
    next_buffer_ = std::make_unique<Buffer>();
    full_buffers_.reserve(DEFAULT_FULLBUFFERS_SIZE);
}

AsyncLogging::~AsyncLogging() {
    if (isRunning()) {
        stop();
    }
}

void AsyncLogging::start() {
    if (!is_running_) {
        is_running_ = true;
        log_thread_.startThread();
    }
}

void AsyncLogging::stop() {
    if (is_running_) {
        is_running_ = false;
        log_thread_.joinThread();
    }
}

void AsyncLogging::append(const char *logmsg, size_t len) {
    std::unique_lock<std::mutex> uniqueLock(mutex_);
    if (curr_buffer_->getAvail() >= len) {
        curr_buffer_->append(logmsg, len);
    } else {
        // 将curr_buffer加入到full_buffer中
        full_buffers_.push_back(std::move(curr_buffer_));
        if (!next_buffer_) {
            next_buffer_ = std::make_unique<Buffer>(); // 新new一个
        }
        curr_buffer_ = std::move(next_buffer_);
        curr_buffer_->append(logmsg, len);
        condition_.notify_all(); // 毕竟只有一个正在条件变量上等待的
    }
}

void AsyncLogging::logThreadFunc() {        // 这个是日志线程的主要逻辑
    BufferPtr new_buffer1 = std::make_unique<Buffer>();
    BufferPtr new_buffer2 = std::make_unique<Buffer>();

    BufferVec output_buffers;
    // output_buffers.reserve(DEFAULT_FULLBUFFERS_SIZE);

    auto wait_condition = [&]()-> bool {
        return !full_buffers_.empty();
    };
    // 关于输出文件的stream
    std::string newlogfile_name = logfile_name_ + "_" +
            TimeStamp::getNowTimeStamp().getTimeFormatString();
    std::ofstream log_output;
    log_output.open(newlogfile_name.c_str(), std::ios::app);    // 只能append
    while (is_running_) {
        {
            std::unique_lock<std::mutex> uniqueLock(mutex_);
            if (full_buffers_.empty()) {
                condition_.wait_for(uniqueLock, flush_interval_ * std::chrono::seconds(1), wait_condition);
            }
            // 将current加入到full_buffers中
            full_buffers_.push_back(std::move(curr_buffer_));
            curr_buffer_ = std::move(new_buffer1);
            if (!next_buffer_) {
                next_buffer_ = std::move(new_buffer2);
            }
            output_buffers.swap(full_buffers_);     // 交换过来, 之后使用output_buffer
        }

        for (auto &output_buffer : output_buffers) {
            // 输出该buffer中的内容
            log_output << output_buffer->getData();
        }

        if (new_buffer1 == nullptr) {
            new_buffer1 = std::make_unique<Buffer>();           // 可以考虑重新分配, 也可以考虑从outputs中交换出来一个
        }
        if (new_buffer2 == nullptr) {
            new_buffer2 = std::make_unique<Buffer>();
        }

        output_buffers.clear();
    }


}