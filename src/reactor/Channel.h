//
// Created by 杨丰硕 on 2023/1/9.
//

#ifndef TINYMUDUO_CHANNEL_H
#define TINYMUDUO_CHANNEL_H

#include <functional>
#include <sys/epoll.h>
#include "EventLoop.h"
#include "../net/Socket.h"

namespace TinyMuduo {
    class EventLoop;
    class Channel {
    public:
        enum ChannelState {
            newChannel = 0,
            addedChannel,
            delChannel,
        };

        using CallBackFunc = std::function<void()>;

        static const uint32_t NULL_EVENT;
        static const uint32_t READ_EVENT;
        static const uint32_t WRITE_EVENT;

        explicit Channel(EventLoop *eventLoop, int fd);

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
            update();
        }

        void setWritable() {
            event_ |= WRITE_EVENT;
            update();
        }

        void update() {
            state_ = addedChannel;
            owner_loop_->updateChannel(this);
        }

        void remove() {
            owner_loop_->removeChannel(this);
            state_ = delChannel;    //  处于移除状态
        }

        bool isInLoop() const {
            return in_loop_;
        }

        void handleEvent();

        ChannelState getState() const {
            return state_;
        }

        void setState(ChannelState state) {
            state_ = state;
        }

        int getFd() const {
            return fd_;
        }

    private:
        bool in_loop_;      // 是否已经加入到loop中
        ChannelState state_;
        EventLoop *owner_loop_;
        uint32_t event_;    // 驱动事件
        uint32_t revent_; // 返回事件
        const int fd_;    // 其实Channel并不是直接暴漏的，而是被封装到了channel里
        // 读、写事件
        CallBackFunc read_callback_;
        CallBackFunc write_callback_;
    };

}

#endif //TINYMUDUO_CHANNEL_H
