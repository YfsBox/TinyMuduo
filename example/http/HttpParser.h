//
// Created by 杨丰硕 on 2023/1/16.
//
#ifndef TINYMUDUO_HTTPPARSER_H
#define TINYMUDUO_HTTPPARSER_H

#include "HttpRequest.h"

namespace TinyMuduo {
    class Buffer;
}

namespace TinyHttp {
    class HttpParser {          // 解析一个buffer从中获取完整的HttpRequest结构
    public:         // 这一部分将会用在TcpConnection中保存
        enum ParserState {
            ParseRequestLine = 0,
            ParseHeaders,
            ParseBody,
            ParseFinish,
        };

        HttpParser():state_(ParserState::ParseRequestLine) {}

        ~HttpParser() = default;

        ParserState getState() const {
            return state_;
        }

        bool isFinishAll() const {
            return state_ == ParserState::ParseFinish;
        }

        bool parsing(TinyMuduo::Buffer *);

    private:

        HttpRequest::Method str2Method(const std::string &mstr);

        void setState(ParserState state) {
            state_ = state;
        }

        bool parseRequestLine(const std::string &line);

        ParserState state_;
        HttpRequest request_;
    };
}

#endif //TINYMUDUO_HTTPPARSER_H
