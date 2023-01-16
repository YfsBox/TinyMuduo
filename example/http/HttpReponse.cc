//
// Created by 杨丰硕 on 2023/1/16.
//
#include <stdio.h>
#include "HttpReponse.h"
#include "../../src/base/Buffer.h"

using namespace TinyHttp;

void HttpReponse::appendToBuffer(TinyMuduo::Buffer *buffer) {
    char tmp_buf[32];
    snprintf(tmp_buf, sizeof(tmp_buf),"HTTP/1.1 %d ", status_code_);
    buffer->append(tmp_buf);
    buffer->append(status_message_);
    buffer->append("\r\n");
    for (auto &[key, value] : head_lines_) {
        buffer->append(key);
        buffer->append(" ");
        buffer->append(value);
        buffer->append("\r\n");
    }
    buffer->append("\r\n");
    buffer->append(body_);
}