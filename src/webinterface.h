#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include <thread>
#include <mutex>
#include <set>

class LicenseManagerInterface;

class WebInterface{

    public:

        WebInterface(int port, LicenseManagerInterface* license_manager);
        virtual ~WebInterface();

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

        void OnOpen(websocketpp::connection_hdl hdl);
        void OnClose(websocketpp::connection_hdl hdl);
        void OnHttp(websocketpp::connection_hdl hdl);

        void HandleHttpMessage(const std::string& method, const std::string& path, const std::string& body,
                HttpResponse& response);

        void HandleGetDongles(HttpResponse& response);
        void HandleGetContext(const std::string& dongle_id, HttpResponse& response);
        void HandleUpdate(const std::string& dongle_id, const std::string& rau_data, HttpResponse& response);
        void HandleGetLicenseCount(const std::string& dongle_id, const std::string& product_id, HttpResponse& response);

        websocketpp::server<websocketpp::config::asio> server_;
        std::thread server_thread_;
        int port_;

        std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl> > connections_;
        std::mutex connection_mutex_;

        LicenseManagerInterface* license_manager_ = NULL;
};

#endif
