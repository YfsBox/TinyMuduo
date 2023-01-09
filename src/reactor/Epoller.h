//
// Created by 杨丰硕 on 2023/1/9.
//

#ifndef TINYMUDUO_EPOLLER_H
#define TINYMUDUO_EPOLLER_H

#include <vector>
#include <sys/epoll.h>

namespace TinyMuduo {

    class Epoller {
    public:
        using EventsVector =  std::vector<epoll_event>;

        Epoller();

        ~Epoller();

        uint16_t getEpollerFd() const {
            return epoller_fd_;
        }



    private:
        uint16_t epoller_fd_;
        EventsVector events_;
    };
}

#endif //TINYMUDUO_EPOLLER_H
