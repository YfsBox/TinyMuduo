//
// Created by 杨丰硕 on 2023/1/12.
//
#include <gtest/gtest.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include "../logger/Logger.h"
#include "../base/Thread.h"
#include "../reactor/Channel.h"

using namespace TinyMuduo;

class TestChannel {
public:

    using ReadFunc = std::function<void()>;

    TestChannel(Channel *channel, uint32_t interval, uint32_t readcnt);

    ~TestChannel();

    void start() {
        read_thread_.startThread();
    }

    void stop() {
        read_thread_.joinThread();
    }

private:

    void readingFunc();     // 周期性地读取channel

    Channel *test_channel_;         // 所要测试的Channel
    Thread read_thread_;
    uint32_t interval_millseconds_;
    uint32_t read_cnt_;
    // ReadFunc read_func_;
};

TestChannel::TestChannel(Channel *channel, uint32_t interval, uint32_t readcnt):
    test_channel_(channel),
    read_thread_(std::bind(&TestChannel::readingFunc, this)),
    interval_millseconds_(interval),
    read_cnt_(readcnt){

}

TestChannel::~TestChannel() {
    if (read_thread_.isStart()) {
        read_thread_.joinThread();
    }
}

void TestChannel::readingFunc() {
    auto tmpcnt = read_cnt_;
    uint64_t num = 1;
    while (tmpcnt) {
        LOG_DEBUG << "begin write to test channel " << test_channel_->getFd() << " wait "
        << interval_millseconds_ << " mills before write";
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_millseconds_));
        ::write(test_channel_->getFd(), &num, sizeof(num));
        tmpcnt--;
    }
}

TEST(CHANNEL_TEST, BASIC_CHANNEL_TEST) {
    // 创建一个Socket及其Channel
    auto socket_fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    Socket socket(socket_fd);
    ASSERT_TRUE(socket.getFd() > 0);
    LOG_DEBUG << "Create a loop, and start a loop thread.";
    EventLoop loop;
    Thread loop_thread([&loop](){
        loop.loop(-1);
    });
    loop_thread.startThread();
    // 创建channel,设置callback,并且注册到loop
    LOG_DEBUG << "Create a channel,set a readable callback for it and then start.";
    Channel channel(&loop, socket.getFd());
    int excepted_cnt = 10;      // 期望的计数为10.
    auto readhandle = [&]() {
        excepted_cnt--;
    };
    // 设置channel可读,并且加入到loop
    channel.setReadCallBack(readhandle);
    channel.setReadable();
    LOG_DEBUG << "Create a test class and then start it.";
    // 启动测试类,用来周期性触发channel事件
    TestChannel testChannel(&channel, 500, 10);
    testChannel.start();
    LOG_DEBUG << "Wait the test class thread and quit the loop.";
    testChannel.stop();
    loop.quit();
    loop_thread.joinThread();
    ASSERT_EQ(excepted_cnt, 0);
    LOG_DEBUG << "test end";
}

TEST(CHANNEL_TEST, MULTI_CHANNEL_TEST) {
    size_t channel_cnt = 50; //同时也是thread cnt，socket cnt
    size_t check_cnt = 10;      // check_cnt
    size_t write_interval = 500;      // mill seconds

    using SocketPtr = std::unique_ptr<Socket>;
    using ChannelPtr = std::unique_ptr<Channel>;
    using TestChannelPtr = std::unique_ptr<TestChannel>;
    using ThreadPtr = std::unique_ptr<Thread>;

    std::vector<SocketPtr> sockets;
    std::vector<ChannelPtr> channels;
    std::vector<TestChannelPtr> testchannels;
    std::vector<ThreadPtr> testthreads;
    // 一个事件循环，创建一个loop线程并启动
    EventLoop loop;
    Thread loop_thread([&loop]() {
        loop.loop(-1);
    });
    loop_thread.startThread();
    // 创建50个socket以及channels
    for (size_t i = 0; i < channel_cnt; ++i) {
        sockets.push_back(std::make_unique<Socket>());
        auto fd = sockets[i]->getFd();
        ASSERT_TRUE(fd > 0);
        channels.push_back(std::make_unique<Channel>(&loop, fd));
    }
    // 用于检查次数的变量
    std::vector<size_t> testcnts;
    testcnts.resize(channel_cnt, check_cnt);
    // 将channels就绪
    for (size_t i = 0; i < channel_cnt; ++i) {
        channels[i]->setReadCallBack([&testcnts, i](){
            testcnts[i]--;
        });
        channels[i]->setReadable();
    }
    // 创建用于触发channel事件的线程
    for (size_t i = 0; i < channel_cnt; ++i) {
        std::function<void()> test_func = [&channels,check_cnt,i,write_interval](){
            size_t write_cnt = check_cnt;
            while (write_cnt) {
                std::this_thread::sleep_for(std::chrono::milliseconds(write_interval));
                ::write(channels[i]->getFd(), "1", 1);
                write_cnt--;
            }
        };
        testthreads.emplace_back(std::make_unique<Thread>(test_func));
    }
    // 启动这些触发器线程
    for (auto &thread : testthreads) {
        thread->startThread();
    }
    // 等待结束
    for (auto &thread : testthreads) {
        thread->joinThread();
    }
    // 检查最终的结果
    loop.quit();
    loop_thread.joinThread();
    for (auto &test_cnt : testcnts) {
        ASSERT_EQ(test_cnt, 0);
    }
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

// epoll不可以监听普通的文件,关于epoll可以监听事件的类型，这个坑等到写完之后再填
