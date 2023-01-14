//
// Created by 杨丰硕 on 2023/1/13.
//
#include <gtest/gtest.h>
#include "../base/Buffer.h"

using namespace TinyMuduo;

TEST(BUFFER_TEST, BASIC_BUFFER_TEST) {

    Buffer buffer;
    ASSERT_EQ(buffer.getReadableSize(), 0);
    ASSERT_EQ(buffer.getWritableSize(), Buffer::INIT_BUFFER_SIZE);
    ASSERT_EQ(buffer.getPrependSize(), Buffer::PREPEND_BUFFER_SIZE);

    std::string test_str1(300, '1');
    buffer.append(test_str1);

    ASSERT_EQ(buffer.getReadableSize(), test_str1.size());
    ASSERT_EQ(buffer.getWritableSize(), Buffer::INIT_BUFFER_SIZE - test_str1.size());
    ASSERT_EQ(buffer.getPrependSize(), Buffer::PREPEND_BUFFER_SIZE);

    std::string test_str2 = buffer.retrieveAsString(50);
    ASSERT_EQ(test_str2, test_str1.substr(0,50));
    ASSERT_EQ(buffer.getReadableSize(), test_str1.size() - 50);
    ASSERT_EQ(buffer.getWritableSize(), Buffer::INIT_BUFFER_SIZE - test_str1.size());
    ASSERT_EQ(buffer.getPrependSize(), Buffer::PREPEND_BUFFER_SIZE + 50);

    std::string test_str3 = buffer.retrieveAsStringAll();
    ASSERT_EQ(test_str3.size(), 250);
    ASSERT_EQ(buffer.getReadableSize(), 0);
    ASSERT_EQ(buffer.getWritableSize(), Buffer::INIT_BUFFER_SIZE - test_str1.size());
    ASSERT_EQ(buffer.getPrependSize(), Buffer::PREPEND_BUFFER_SIZE + test_str1.size());

}

TEST(BUFFER_TEST, BUFFER_GROW_TEST) {
    Buffer buffer;
    size_t init_size = 1024;

    for (size_t i = 0; i < 4; ++i) {
        init_size *= 2;
        std::string tmp_str(init_size, '2');
        buffer.append(tmp_str);
        ASSERT_EQ(buffer.getReadableSize(), init_size);
        ASSERT_EQ(buffer.getWritableSize(), 0);
        ASSERT_EQ(buffer.getPrependSize(), Buffer::PREPEND_BUFFER_SIZE);
        buffer.retrieveAll();
    }
    // std::string retrieve_str = buffer.retrieveAsStringAll();
    ASSERT_EQ(buffer.getReadableSize(), 0);
    ASSERT_EQ(buffer.getWritableSize(), init_size);
    ASSERT_EQ(buffer.getPrependSize(), Buffer::PREPEND_BUFFER_SIZE);
}

TEST(BUFFER_TEST, BUFFER_INSIDEGROW_TEST) {
    Buffer buffer;

    std::string str1(768, '1');
    buffer.append(str1);
    ASSERT_EQ(buffer.getReadableSize(), 768);
    ASSERT_EQ(buffer.getWritableSize(), 256);
    ASSERT_EQ(buffer.getPrependSize(), Buffer::PREPEND_BUFFER_SIZE);

    buffer.retrieve(256);
    std::string str2(512, '2');
    buffer.append(str2);        // 会发生内部扩容
    ASSERT_EQ(buffer.getReadableSize(), 1024);
    ASSERT_EQ(buffer.getWritableSize(), 0);
    ASSERT_EQ(buffer.getPrependSize(), Buffer::PREPEND_BUFFER_SIZE);

}


int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

// 为什么需要用户层缓冲区,缓冲区在读写时饿工作流程(写一些测试代码体会这个过程)


