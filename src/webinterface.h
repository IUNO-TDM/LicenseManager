#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include "regexwrapper.h"

#include <thread>
#include <mutex>
#include <set>

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

class LicenseManagerInterface;

class WebInterface{

    public:

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

        WebInterface(int port, LicenseManagerInterface* lm);
        virtual ~WebInterface();

        void HandleHttpMessage(const std::string& method, const std::string& path, const std::string& body,
                HttpResponse& response);

    private:

        void HandleGetDongles(HttpResponse& response);
        void HandleGetContext(const std::string& dongle_id, HttpResponse& response);
        void HandleUpdate(const std::string& dongle_id, const std::string& rau_data, HttpResponse& response);
        void HandleGetLicenseCount(const std::string& dongle_id, const std::string& product_id, HttpResponse& response);
        void HandleGetLicenses(const std::string& dongle_id, HttpResponse& response);
       

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

        LicenseManagerInterface* license_manager_;

        Pistache::Http::Endpoint* server_;
};


#endif
