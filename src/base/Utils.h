//
// Created by 杨丰硕 on 2023/1/8.
//

#ifndef TINYMUDUO_UTILS_H
#define TINYMUDUO_UTILS_H

#include <iostream>

class DebugUtil {
public:
    explicit DebugUtil(const std::string &msg);
    ~DebugUtil();

    DebugUtil(const DebugUtil& debug) = delete;
    DebugUtil& operator=(const DebugUtil& debug) = delete;

private:
    std::string msg_;
};

DebugUtil::DebugUtil(const std::string &msg):msg_(msg) {
    std::cout << "Debug begin: " << msg_ << '\n';
}

DebugUtil::~DebugUtil() {
    std::cout << "Debug end: " << msg_ << '\n';
}



#endif //TINYMUDUO_UTILS_H
