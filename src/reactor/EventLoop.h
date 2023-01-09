//
// Created by 杨丰硕 on 2023/1/8.
//

#ifndef TINYMUDUO_EVENTLOOP_H
#define TINYMUDUO_EVENTLOOP_H

#include <functional>
#include <vector>
#include <list>
#include <memory>
#include <mutex>
#include <atomic>

namespace TinyMuduo {
    class Epoller;
    class Channel;
    class EventLoop {
    public:
        using ChannelVector = std::vector<Channel*>;
        using QueuedFunctor = std::function<void()>;

        EventLoop();

        ~EventLoop();

        void loop(int timeout);

        void quit(); // 通常会被外部调用,用来关闭该loop

    private:
        std::atomic_bool is_looping_;
        std::atomic_bool is_quit_;
        const pid_t thread_id_;
        std::mutex mutex_;
        std::unique_ptr<Epoller> epoller_;
        ChannelVector active_channels_;     // 用于存放从epoller中所读取的channels
        std::list<QueuedFunctor> queued_list_;
    };
}

#endif //TINYMUDUO_EVENTLOOP_H
