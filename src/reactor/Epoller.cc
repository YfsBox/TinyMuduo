//
// Created by 杨丰硕 on 2023/1/9.
//
#include <unistd.h>
#include "Epoller.h"
#include "Channel.h"

using namespace TinyMuduo;

Epoller::Epoller():epoller_fd_(::epoll_create1(EPOLL_CLOEXEC)) {    // 其中EPOLL_CLOEXEC表示的是该描述符是具有
    // O_CLOEXEC的特性的
    events_.resize(INIT_EVENT_SIZE);    //  设置event数组
}

Epoller::~Epoller() {
    ::close(epoller_fd_);
}

uint16_t Epoller::epoll(int timeout_second, ChannelVector &channel_list) {
    auto timeout_millsecond = timeout_second * MillSecondsPerSecond;        // epoll中的单位是mill second，要先转化一下

    uint16_t epoll_cnt = ::epoll_wait(epoller_fd_, &*events_.begin(),
                                 static_cast<int>(events_.size()), timeout_millsecond);    // max_size就是一次读取的最大事件数

    for (size_t i = 0; i < epoll_cnt; ++i) {       // 这里有一个地方需要注意, 要用的是epoll_cnt,而不是直接遍历events
        // 从中读取指针，其中藏着Channel的指针
        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevent(events_[i].events);
        channel_list.push_back(static_cast<Channel*>(channel));  // 将Channel追加进入
    }
    return epoll_cnt;
}



