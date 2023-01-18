//
// Created by 杨丰硕 on 2023/1/16.
//

#ifndef TINYMUDUO_HTTPREPONSE_H
#define TINYMUDUO_HTTPREPONSE_H

#include <map>
#include "../../src/base/Buffer.h"

namespace TinyHttp {

    class HttpReponse {
    public:
        using HeadLines = std::map<std::string, std::string>;

        enum Version {
            VInvalid, VHttp11,
        };

        enum HttpStatusCode {
            Unknown,
            Ok200 = 200,
            MovedPermantly301 = 301,
            BadRequest400 = 400,
            NotFound404 = 404,
        };

        HttpReponse():version_(Version::VInvalid),
            status_code_(HttpStatusCode::Unknown) {

        }

        explicit HttpReponse(bool close):
            close_connection_(close),
            version_(Version::VInvalid),
            status_code_(HttpStatusCode::Unknown)
            {}


        ~HttpReponse() = default;

        void appendToBuffer(TinyMuduo::Buffer *buffer);

        void setVersion(Version version) {
            version_ = version;
        }

        void setHttpStatusCode(HttpStatusCode statusCode) {
            status_code_ = statusCode;
        }

        void setStatusMessage(const std::string &msg) {
            status_message_ = msg;
        }

        void setBody(const std::string &body) {
            body_ = body;
        }

        void setCloseConnection(bool close) {
            close_connection_ = close;
        }

        bool getCloseConnection() const {
            return close_connection_;
        }

        void addHeaderKv(const std::string &key, const std::string &value) {
            head_lines_[key] = value;
        }

        void setContentType(const std::string &type) {
            addHeaderKv("Content-Type", type);
        }

    private:
        bool close_connection_;
        Version version_;
        HttpStatusCode status_code_;
        std::string status_message_;
        HeadLines head_lines_;
        std::string body_;
    };

}

#endif //TINYMUDUO_HTTPREPONSE_H
