//
// Created by 杨丰硕 on 2023/1/11.
//
#include "test_utils.h"

WaitThread::WaitThread(uint32_t waitseconds):
    wait_seconds_(waitseconds),
    thread_(std::bind(&WaitThread::waitFunc, this)){

}


