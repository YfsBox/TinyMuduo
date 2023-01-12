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
        // 设置一些Callback,目前只实现了读写相关的
        void setReadCallBack(CallBackFunc cb) {
            read_callback_ = cb;
        }

        void setWriteCallBack(CallBackFunc cb) {
            write_callback_ = cb;
        }
        // 获取事件
        uint32_t getEvent() const {
            return event_;
        }

        uint32_t getRevent() const {
            return revent_;
        }

        void setEvent(uint32_t event) {
            event_ = event;
            update();
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
            // LOG_DEBUG << "The event is " << event_ << " before";
            event_ |= READ_EVENT;
            // LOG_DEBUG << "The event is " << event_ << " and the READ_EVENT is " << READ_EVENT;
            update();
        }

        void setWritable() {
            event_ |= WRITE_EVENT;
            update();
        }

        void update() {
            // state_ = addedChannel;
            owner_loop_->updateChannel(this);
        }

        void remove() {
            owner_loop_->removeChannel(this);
            // state_ = delChannel;    //  处于移除状态
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
    //  一般的使用方式:首先设置callback，然后设置writable、readable。
    // writable和readable中有update，update间接地借助loop调用了epoller中的updatechannel

    // 如果一个channel处于创建之初,也就是new,则是执行加入map以及EPOLL_CTL_ADD等操作
}

#endif //TINYMUDUO_CHANNEL_H
