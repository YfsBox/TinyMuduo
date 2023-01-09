//
// Created by 杨丰硕 on 2023/1/9.
//

#ifndef TINYMUDUO_CHANNEL_H
#define TINYMUDUO_CHANNEL_H

#include <functional>
#include <sys/epoll.h>
#include "../net/Socket.h"

namespace TinyMuduo {
    class EventLoop;
    class Channel {
    public:
        using CallBackFunc = std::function<void()>;

        static const uint32_t NULL_EVENT;
        static const uint32_t READ_EVENT;
        static const uint32_t WRITE_EVENT;

        explicit Channel(EventLoop *eventLoop);

        Channel();

        ~Channel();

        void setReadCallBack(CallBackFunc cb) {
            read_callback_ = cb;
        }

        void setWriteCallBack(CallBackFunc cb) {
            write_callback_ = cb;
        }

        uint32_t getEvent() const {
            return event_;
        }

        uint32_t getRevent() const {
            return revent_;
        }

        void setEvent(uint32_t event) {
            event_ = event;
        }

        void setRevent(uint32_t revent) {
            revent_ = revent;
        }

        bool isReadable() const {
            return event_ & READ_EVENT;
        }

        bool isWritable() const {
            return event_ & WRITE_EVENT;
        }

        void setReadable() {
            event_ |= READ_EVENT;
        }

        void setWritable() {
            event_ |= WRITE_EVENT;
        }

        void handleEvent();

    private:
        EventLoop *owner_loop_;
        uint32_t event_;    // 驱动事件
        uint32_t revent_; // 返回事件
        Socket socket_;     // 其实Channel并不是直接暴漏的，而是被封装到了channel里
        // 读、写事件
        CallBackFunc read_callback_;
        CallBackFunc write_callback_;
    };

}

#endif //TINYMUDUO_CHANNEL_H
