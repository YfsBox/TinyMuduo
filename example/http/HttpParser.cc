//
// Created by 杨丰硕 on 2023/1/16.
//
#include "HttpParser.h"
#include "../../src/base/Buffer.h"
#include "../../src/logger/Logger.h"

using namespace TinyHttp;

HttpRequest::Method HttpParser::str2Method(const std::string &mstr) {
    if (mstr == "POST") {
        return HttpRequest::MPost;
    } else if (mstr == "GET") {
        return HttpRequest::MGet;
    } else {
        return HttpRequest::MInvalid;
    }
}


bool HttpParser::parseRequestLine(const std::string &line) {
    // 以空格为单位逐个分割
    auto start = line.begin();
    for (size_t i = 0; i < 3; ++i) {
        auto space = std::find(start, line.end(), ' ');
        if (space == line.end() && i != 2) {
            return false;
        }
        std::string field(start, space);
        switch (i) {
            case 0:{
                auto method = str2Method(field);
                if (method == HttpRequest::MInvalid) {
                    return false;
                }
                LOG_DEBUG << "method:" << field;
                request_.setMethod(method);
                break;
            }
            case 1:{
                LOG_DEBUG << "url:" << field;
                request_.setUrl(field);
                break;
            }
            case 2:{
                LOG_DEBUG << "version:" << field;
                // 首先判断长度是否合法
                if (field.size() != 8 || !std::equal(start, space - 1, "HTTP/1.")) {        // 版本号的标准长度
                    return false;
                }
                char lastchar = field.back();
                HttpRequest::Version ver;
                if (lastchar == '1') {
                    ver = HttpRequest::VHttp11;
                } else if (lastchar == '0') {
                    ver = HttpRequest::VHttp10;
                } else {
                    ver = HttpRequest::VInvalid;
                }
                request_.setVersion(ver);
                break;
            }
        }
        start = space + 1;
    }

    return true;
}

bool HttpParser::parsing(TinyMuduo::Buffer *buffer) {
    bool parsing = true;
    bool is_ok = true;
    while (parsing) {       // 一个用来解析request内容的状态机
        // 首先以CRLF为边界,解析出一行信息
        auto crlf = buffer->findCRLF();       // 从头开始找吧
        if (crlf == nullptr) {      // 已经找不到crlf了
            break;
        }
        std::string line(buffer->retrieveUtil(crlf));       // 获取这一行消息
        switch (state_) {
            case ParserState::ParseRequestLine: {
                bool parse_ok = parseRequestLine(line);
                if (parse_ok) {
                    setState(ParserState::ParseHeaders);
                } else {
                    is_ok = false;
                }
                break;
            }
            case ParserState::ParseHeaders: {
                // 首先分割出冒号
                auto colon = std::find(line.begin(), line.end(), ':');
                if (colon == line.end()) {      // 如果找不到冒号
                    setState(ParserState::ParseBody);

                } else {        // 如果能够找到冒号
                    std::string key(line.begin(), colon);
                    std::string value(colon + 1, line.end());
                    LOG_DEBUG << "{" << key << "," << value << "}";     // 调试看结果
                    request_.addHeaderKv(key, value);
                }
                break;
            }
            default:
                parsing = false;
                break;
        }
    }
    return is_ok;
}
