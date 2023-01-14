//
// Created by 杨丰硕 on 2023/1/9.
//
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "Epoller.h"
#include "Channel.h"
#include "../logger/Logger.h"
#include "../base/Timestamp.h"

using namespace TinyMuduo;

Epoller::Epoller():epoller_fd_(::epoll_create1(EPOLL_CLOEXEC)) {    // 其中EPOLL_CLOEXEC表示的是该描述符是具有
    // O_CLOEXEC的特性的
    if (epoller_fd_ < 0) {
        LOG_ERROR << "epoll fd create error";
    }
    LOG_DEBUG << "Epoller create epoller fd is " << epoller_fd_;
    events_.resize(INIT_EVENT_SIZE);    //  设置event数组
}

Epoller::~Epoller() {
    ::close(epoller_fd_);
}

TimeStamp Epoller::epoll(int timeout_second, ChannelVector &channel_list) {

    auto timeout_millsecond = timeout_second;        // epoll中的单位是mill second，要先转化一下
    if (timeout_millsecond > 0) {
        timeout_millsecond = timeout_second * static_cast<int>(TimeStamp::MillSecondsPerSecond);
    }           // 如果是-1000就会出现反复频繁触发的问题

    uint16_t epoll_cnt = ::epoll_wait(epoller_fd_, &*events_.begin(),
                                 static_cast<int>(events_.size()), timeout_millsecond);    // max_size就是一次读取的最大事件数
    TimeStamp now_time = TimeStamp::getNowTimeStamp();

    /*LOG_INFO << "epoll_wait return " << epoll_cnt << " events at time "
    << now_time.getTimeFormatString() << " on epoll_fd " << epoller_fd_;*/
    for (size_t i = 0; i < epoll_cnt; ++i) {       // 这里有一个地方需要注意, 要用的是epoll_cnt,而不是直接遍历events
        // 从中读取指针，其中藏着Channel的指针
        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevent(events_[i].events);
        channel_list.push_back(channel);  // 将Channel追加进入
    }

    if (epoll_cnt == events_.size()) {  // 需要扩容,借助vector的可扩容机制
        events_.resize(2 * epoll_cnt);
    }
    return now_time;

}
// 其实这里也还是为了统一接口,这一部分可以分开写,分别写一个DEL、ADD还有MOD
void Epoller::updateChannel(Channel *channel) {
    auto state = channel->getState();
    int channel_fd = channel->getFd();
    if (state == Channel::delChannel || state == Channel::newChannel) {
        if (state == Channel::newChannel) {
            auto find_channel = channels_map_.find(channel_fd);
            assert(find_channel == channels_map_.end());        // 不可以之前已经加入过
            if (find_channel != channels_map_.end()) {
                channels_map_[channel_fd] = channel;
            }
        }

        channel->setState(Channel::addedChannel);
        update(EPOLL_CTL_ADD, channel);
        LOG_DEBUG << "add channel(fd: " << channel->getFd() << ") ok";

    } else {
        assert(channels_map_.find(channel_fd) == channels_map_.end());
        assert(channel != channels_map_[channel_fd]);

        if (channel->getEvent() == Channel::NULL_EVENT) {  // 如果是空的interested事件
            update(EPOLL_CTL_DEL, channel);
            channel->setState(Channel::ChannelState::delChannel);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void Epoller::removeChannel(Channel *channel) {
    auto state = channel->getState();
    int fd = channel->getFd();
    assert(state != Channel::newChannel);
    auto findit = channels_map_.find(fd);
    assert(findit != channels_map_.end());
    channels_map_.erase(findit);    // 移除

    if (state == Channel::addedChannel) {
        update(EPOLL_CTL_DEL, channel);
    }

    channel->setState(Channel::newChannel);
}

void Epoller::update(int32_t op, Channel *channel) {
    struct epoll_event event;

    memset(&event, 0, sizeof(event));
    event.events = channel->getEvent();
    event.data.ptr = channel;

    int ret = ::epoll_ctl(epoller_fd_, op, channel->getFd(), &event);

    if (ret <  0) {
        LOG_ERROR << "epoll ctl " << channel->getFd() << " error, epoller fd is " <<
        epoller_fd_ << ", the errorno is " << errno;
    }
}




