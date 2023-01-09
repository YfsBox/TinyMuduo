//
// Created by 杨丰硕 on 2023/1/9.
//

#ifndef TINYMUDUO_EPOLLER_H
#define TINYMUDUO_EPOLLER_H

#include <vector>
#include <sys/epoll.h>

namespace TinyMuduo {

    class Channel;
    class Epoller {
    public:
        using EventsVector = std::vector<epoll_event>;

        using ChannelVector = std::vector<Channel*>;

        static const uint16_t INIT_EVENT_SIZE = 16;

        static const uint16_t MillSecondsPerSecond = 1000;

        Epoller();

        ~Epoller();

        uint16_t getEpollerFd() const {
            return epoller_fd_;
        }

        uint16_t epoll(int timeout_second, ChannelVector &channel_list); // 调用epoll_wait轮询, 返回值表示的有响应的事件数量

    private:
        uint16_t epoller_fd_;
        EventsVector events_;
    };
}

#endif //TINYMUDUO_EPOLLER_H
