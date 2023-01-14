//
// Created by 杨丰硕 on 2023/1/13.
//
#include <gtest/gtest.h>
#include "../base/Buffer.h"



int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}

// 为什么需要用户层缓冲区,缓冲区在读写时饿工作流程(写一些测试代码体会这个过程)


