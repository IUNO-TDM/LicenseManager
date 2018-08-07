#ifndef HTTPHANLDER_H
#define HTTPHANLDER_H

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include "regexwrapper.h"

class LicenseManagerInterface;
class HttpHandler : public Pistache::Http::Handler {

    public:
        HTTP_PROTOTYPE(HttpHandler)

        HttpHandler();
        void onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response);

    private:
        struct HttpResponse {
                int response_code_;
                std::string response_message_;
                std::string content_type_;
                void Set(int code, const char* msg){
                    response_code_=code;
                    response_message_ = msg;
                }
                void Set(int code, const std::string& msg){
                    response_code_ = code;
                    response_message_ = msg;
                }
                void SetContentType(const char* content_type){
                    content_type_ = content_type;
                }
        };
        void HandleHttpMessage(const std::string& method, const std::string& path, const std::string& body,
                HttpResponse& response);

        void HandleGetDongles(HttpResponse& response);
        void HandleGetContext(const std::string& dongle_id, HttpResponse& response);
        void HandleUpdate(const std::string& dongle_id, const std::string& rau_data, HttpResponse& response);
        void HandleGetLicenseCount(const std::string& dongle_id, const std::string& product_id, HttpResponse& response);
        void HandleGetLicenses(const std::string& dongle_id, HttpResponse& response);

        LicenseManagerInterface* license_manager_ = NULL;

        RegexWrapper rxHandleGetDongles_;
        RegexWrapper rxHandleGetContext_;
        RegexWrapper rxHandleUpdate_;
        RegexWrapper rxHandleGetLicenseCount_;
        RegexWrapper rxHandleGetLicenses_;
        RegexWrapper rxHandleGetDongles405_;
        RegexWrapper rxHandleGetContext405_;
        RegexWrapper rxHandleUpdate405_;
        RegexWrapper rxHandleGetLicenseCount405_;
        RegexWrapper rxHandleGetLicenses405_;
};

#endif