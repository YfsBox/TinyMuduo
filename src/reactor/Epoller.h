//
// Created by 杨丰硕 on 2023/1/9.
//

#ifndef TINYMUDUO_EPOLLER_H
#define TINYMUDUO_EPOLLER_H

#include <vector>
#include <map>
#include <sys/epoll.h>

namespace TinyMuduo {

    class Channel;
    class TimeStamp;
    class Epoller {
    public:
        using EventsVector = std::vector<epoll_event>;

        using ChannelVector = std::vector<Channel*>;

        using ChannelsMap = std::map<int, Channel*>;

        static const uint16_t INIT_EVENT_SIZE = 16;

        static const uint16_t MillSecondsPerSecond = 1000;

        Epoller();

        ~Epoller();

        uint16_t getEpollerFd() const {
            return epoller_fd_;
        }

        TimeStamp epoll(int timeout_second, ChannelVector &channel_list); // 调用epoll_wait轮询, 返回值表示的有响应的事件数量

        void updateChannel(Channel *channel);

        void removeChannel(Channel *channel);

    private:

        void update(int32_t op, Channel *channel);

        uint16_t epoller_fd_;
        EventsVector events_;
        ChannelsMap channels_map_;
    };
}

#endif //TINYMUDUO_EPOLLER_H
