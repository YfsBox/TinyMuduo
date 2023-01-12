set(SRCS
        base/Timestamp.cc
        base/Thread.cc
        base/Threadpool.cc
        base/CountDownLatch.cc
        base/Buffer.cc
        logger/AsyncLogging.cc
        logger/LoggerStream.cc
        logger/Logger.cc
        net/SockAddress.cc
        net/Socket.cc
        reactor/Channel.cc
        reactor/Epoller.cc
        reactor/EventLoop.cc
        )

# main library
add_library(tinymuduo ${SRCS})
target_link_libraries(tinymuduo rt pthread)

# for test
add_executable(time_test test/time_test.cc test/test_utils.cc)
target_link_libraries(time_test tinymuduo gtest)

add_executable(thread_test test/thread_test.cc test/test_utils.cc)
target_link_libraries(thread_test tinymuduo gtest)

add_executable(socket_test test/socket_test.cc test/test_utils.cc)
target_link_libraries(socket_test tinymuduo gtest)

add_executable(eventloop_test test/eventloop_test.cc test/test_utils.cc)
target_link_libraries(eventloop_test tinymuduo gtest)

add_executable(log_test test/log_test.cc test/test_utils.cc)
target_link_libraries(log_test tinymuduo gtest)

add_executable(channel_test test/channel_test.cc test/test_utils.cc)
target_link_libraries(channel_test tinymuduo gtest)
