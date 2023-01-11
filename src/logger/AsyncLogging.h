//
// Created by 杨丰硕 on 2023/1/10.
//

#ifndef TINYMUDUO_ASYNCLOGGING_H
#define TINYMUDUO_ASYNCLOGGING_H

#include <atomic>
#include <mutex>
#include <memory>
#include <condition_variable>
#include "../base/Thread.h"
#include "LoggerStream.h"

namespace TinyMuduo {
    class AsyncLogging: Utils::noncopyable {
    public:

        static const uint32_t DEFAULT_INTERVAL = 3;

        static const uint32_t DEFAULT_FULLBUFFERS_SIZE = 16;

        static const std::string DEFAULT_LOGTHREAD_NAME;

        AsyncLogging() = default;

        ~AsyncLogging() = default;

        static AsyncLogging& getInstance() {
            static AsyncLogging instance;
            return instance;
        }

        void init(uint32_t interval = DEFAULT_INTERVAL,
                  const std::string &filename = DEFAULT_LOGTHREAD_NAME);

        void start();

        void stop();

        void append(const char *logmsg, size_t len);  //  提供给前台线程输出日志的接口

        bool isRunning() const {
            return is_running_;
        }

    private:
        using Buffer = FixedBuffer<SmallBufferSize>;
        using BufferPtr = std::unique_ptr<Buffer>;
        using BufferVec = std::vector<BufferPtr>;

        void logThreadFunc();

        std::atomic_bool is_running_;
        uint32_t flush_interval_;
        std::string logfile_name_;
        std::unique_ptr<Thread> log_thread_;     // 日志输出所需要的线程
        std::mutex mutex_;
        std::condition_variable condition_;

        BufferPtr curr_buffer_;
        BufferPtr next_buffer_;
        BufferVec full_buffers_;

    };

}

#endif //TINYMUDUO_ASYNCLOGGING_H
