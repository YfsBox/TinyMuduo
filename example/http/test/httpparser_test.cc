//
// Created by 杨丰硕 on 2023/1/16.
//
#include <memory>
#include <gtest/gtest.h>
#include "../HttpParser.h"
#include "../../../src/base/Buffer.h"
#include "../../../src/logger/Logger.h"


namespace TinyHttp {

    class TestParser {
    public:
        TestParser() : parser_(std::make_unique<HttpParser>()) {}

        ~TestParser() = default;

        void testParserRequestLine(const std::string &line) {
            parser_->parseRequestLine(line);
        }

        void testParserRequest(const std::string &request) {
            TinyMuduo::Buffer buffer;
            buffer.append(request);
            parser_->parsing(&buffer);
        }

        HttpRequest::Version getVersion() const {
            return parser_->request_.getVersion();
        }

        HttpRequest::Method getMethod() const {
            return parser_->request_.getMethod();
        }

        std::string getUrl() const {
            return parser_->request_.getUrl();
        }

        HttpRequest::HeaderLines getHeaderLines() const {
            return parser_->request_.getHeaderLines();
        }

        bool testFinishAll() const {
            return parser_->isFinishAll();
        }

    private:
        std::unique_ptr<HttpParser> parser_;
    };

}


TEST(HTTPPARSER_TEST, PARSER_REQUESTLINE_TEST) {
    TinyHttp::TestParser tester;
    std::string line1("GET / HTTP/1.1");
    tester.testParserRequestLine(line1);

    ASSERT_EQ(tester.getMethod(), TinyHttp::HttpRequest::Method::MGet);
    ASSERT_EQ(tester.getVersion(), TinyHttp::HttpRequest::Version::VHttp11);
    ASSERT_EQ(tester.getUrl(), "/");

    std::string line2("POST /chapter17/user.html HTTP/1.0");
    tester.testParserRequestLine(line2);
    ASSERT_EQ(tester.getMethod(), TinyHttp::HttpRequest::Method::MPost);
    ASSERT_EQ(tester.getVersion(), TinyHttp::HttpRequest::Version::VHttp10);
    ASSERT_EQ(tester.getUrl(), "/chapter17/user.html");

}

TEST(HTTPPARSER_TEST, PARSER_PARSEHEADERS_TEST) {
    TinyHttp::TestParser tester;
    std::string msg_str;
    msg_str.append("POST /chapter17/user.html HTTP/1.0\r\n");
    std::vector<std::string> header_keys = {
            "Host", "Accept", "Pragma", "Cache-Control", "Referer", "User-Agent", "Range",
    };
    std::vector<std::string> header_values = {
            "download.google.com", "*/*", "no-cache", "no-cache", "http://download.google.com/",
            "Mozilla/4.04[en](Win95;I;Nav)", "bytes=554554-",
    };
    size_t header_size = header_keys.size();
    std::string headers_str;
    for (size_t i = 0; i < header_size; ++i) {
        char buf[64];
        sprintf(buf,"%s:%s\r\n", header_keys[i].c_str(), header_values[i].c_str());
        msg_str.append(std::string(buf));
    }
    // LOG_DEBUG << headers;
    tester.testParserRequest(msg_str);
    ASSERT_EQ(tester.getMethod(), TinyHttp::HttpRequest::Method::MPost);
    ASSERT_EQ(tester.getUrl(), "/chapter17/user.html");
    ASSERT_EQ(tester.getVersion(), TinyHttp::HttpRequest::Version::VHttp10);
    // 检查header相关的部分
    auto headlines = tester.getHeaderLines();
    /*for (auto &[key, value] : headlines) {
        LOG_DEBUG << "{" << key << "," << value << "}";
    }*/
    for (size_t i = 0; i < header_size; ++i) {
        auto tmp_key = header_keys[i];
        ASSERT_EQ(headlines[tmp_key], header_values[i]);
    }

    ASSERT_EQ(tester.testFinishAll(), true);

}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}


