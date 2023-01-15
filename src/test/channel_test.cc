//
// Created by 杨丰硕 on 2023/1/12.
//
#include <gtest/gtest.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include "../logger/Logger.h"
#include "../base/Thread.h"
#include "../reactor/EventLoopThread.h"
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

TEST(CHANNEL_TEST, BASIC_CHANNEL_TEST) {        // 相当于loop中的wakeup
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
    auto readhandle = [&excepted_cnt, socket_fd]() {
        uint64_t num;
        ssize_t ret = ::read(socket_fd, &num, sizeof(num));
        ASSERT_TRUE(ret >= 0);
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
    loop.wakeupAndQuit();
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
        int tmp_fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        ASSERT_TRUE(tmp_fd > 0);
        sockets.push_back(std::make_unique<Socket>(tmp_fd));
        channels.push_back(std::make_unique<Channel>(&loop, tmp_fd));
    }
    // 用于检查次数的变量
    std::vector<size_t> testcnts;
    testcnts.resize(channel_cnt, check_cnt);
    // 将channels就绪
    for (size_t i = 0; i < channel_cnt; ++i) {
        channels[i]->setReadCallBack([&testcnts, i, &channels](){
            uint64_t num;
            ssize_t ret = ::read(channels[i]->getFd(), &num, sizeof(num));
            ASSERT_TRUE(ret >= 0);
            testcnts[i]--;
        });
        channels[i]->setReadable();
    }
    // 创建用于触发channel事件的线程
    for (size_t i = 0; i < channel_cnt; ++i) {
        std::function<void()> test_func = [&channels, check_cnt, i, write_interval](){
            size_t write_cnt = check_cnt;
            while (write_cnt) {
                uint64_t num = 1;
                std::this_thread::sleep_for(std::chrono::milliseconds(write_interval));
                ::write(channels[i]->getFd(), &num, sizeof(num));
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
    loop.wakeupAndQuit();
    loop_thread.joinThread();
    for (auto &test_cnt : testcnts) {
        ASSERT_EQ(test_cnt, 0);
    }
}

TEST(CHANNEL_TEST, SIMPLE_WRITE_TEST) {         // 这组test点用来观察可写channel在epoller中的反应
    auto tmp_fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    EventLoop owner_loop;
    EventLoopThread loop_thread("test loop thread");
    auto io_loop = loop_thread.startAndGetEventLoop();
    auto channel = std::make_shared<Channel>(io_loop, tmp_fd);
    channel->setWriteCallBack([](){
        LOG_DEBUG << "A write event happened";
    });
    channel->setWritable();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // 结果表明,在水平触发的epoll中，只要tcp buffer没有满，就一直返回可写事件
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

// epoll不可以监听普通的文件,关于epoll可以监听事件的类型，这个坑等到写完之后再填

// 起初编写测试的问题在于，没有再eventfd的read回调中调用read，这使得epoll所获取的可读事件一直没有被读
// 从而一直处于可读状态，导致不停地触发，这也是水平触发的特点，即使一个事件没有在第一事件被处理，那么接下来的epoll_wait中
// 仍然会返回之前没有处理完的事件