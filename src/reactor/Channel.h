//
// Created by 杨丰硕 on 2023/1/9.
//

#ifndef TINYMUDUO_CHANNEL_H
#define TINYMUDUO_CHANNEL_H

#include <sys/epoll.h>
#include "../net/Socket.h"

namespace TinyMuduo {
    class Channel {
    public:

        Channel();

        ~Channel();




    private:
        uint32_t event_;    // 驱动事件
        uint32_t revent_; // 返回事件
        Socket socket_;     // 其实Channel并不是直接暴漏的，而是被封装到了channel里
    };

}

#endif //TINYMUDUO_CHANNEL_H
