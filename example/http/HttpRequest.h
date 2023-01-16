//
// Created by 杨丰硕 on 2023/1/16.
//

#ifndef TINYMUDUO_HTTPREQUEST_H
#define TINYMUDUO_HTTPREQUEST_H

#include <iostream>
#include <map>

#include "../../src/base/Timestamp.h"

namespace TinyHttp {
    class HttpRequest {
    public:
        using HeaderLines = std::map<std::string, std::string>;

        enum Method {
            MInvalid, MGet, MPost
        };

        enum Version {
            VInvalid, VHttp11
        };

        HttpRequest():method_(Method::MInvalid),
            version_(Version::VInvalid){

        }

        ~HttpRequest() = default;

        Method getMethod() const {
            return method_;
        }

        std::string getUrl() const {
            return url_;
        }

        Version getVersion() const {
            return version_;
        }

        std::string getBody() const {
            return body_;
        }

        void setMethod(Method method) {
            method_ = method;
        }

        void setVersion(Version version) {
            version_ = version;
        }

        void setUrl(const std::string &url) {
            url_ = url;
        }

        void setBody(const std::string &body) {
            body_ = body;
        }

    private:

        Method method_;
        Version version_;
        std::string url_;
        std::string body_;
        HeaderLines headers_;

        TinyMuduo::TimeStamp timestamp_;

    };


}


#endif //TINYMUDUO_HTTPREQUEST_H
