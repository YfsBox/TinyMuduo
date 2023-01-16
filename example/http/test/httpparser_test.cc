//
// Created by 杨丰硕 on 2023/1/16.
//
#include <memory>
#include <gtest/gtest.h>
#include "../HttpParser.h"


namespace TinyHttp {

    class TestParser {
    public:
        TestParser() : parser_(std::make_unique<HttpParser>()) {}

        ~TestParser() = default;

        void testParserRequestLine(const std::string &line) {
            parser_->parseRequestLine(line);
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

}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}


