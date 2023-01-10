//
// Created by 杨丰硕 on 2023/1/10.
//
#include "AsyncLogging.h"

using namespace TinyMuduo;

const std::string AsyncLogging::DEFAULT_LOGTHREAD_NAME = "Log Backend Thread";


AsyncLogging::AsyncLogging(uint32_t interval, const std::string &filename):
    is_running_(false),
    flush_interval_(interval),
    logfile_name_(filename),
    log_thread_(std::bind(&AsyncLogging::logThreadFunc, this),
                DEFAULT_LOGTHREAD_NAME) {

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
