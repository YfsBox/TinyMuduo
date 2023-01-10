//
// Created by 杨丰硕 on 2023/1/10.
//

#ifndef TINYMUDUO_ASYNCLOGGING_H
#define TINYMUDUO_ASYNCLOGGING_H

#include <atomic>
#include "../base/Thread.h"

namespace TinyMuduo {
    class AsyncLogging {
    public:

        static const uint32_t DEFAULT_INTERVAL = 3;

        static const std::string DEFAULT_LOGTHREAD_NAME;

        explicit AsyncLogging(uint32_t interval = DEFAULT_INTERVAL,
                              const std::string &filename = DEFAULT_LOGTHREAD_NAME);

        ~AsyncLogging();

        void start();

        void stop();

        bool isRunning() const {
            return is_running_;
        }

    private:

        void logThreadFunc();

        std::atomic_bool is_running_;
        uint32_t flush_interval_;
        std::string logfile_name_;
        Thread log_thread_;     // 日志输出所需要的线程
    };

}

#endif //TINYMUDUO_ASYNCLOGGING_H
