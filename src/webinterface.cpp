#include "webinterface.h"

#include "licensemanagerinterface.h"

#include "easylogging++.h"

#include <boost/regex.hpp>
#include <sstream>

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using namespace std;

WebInterface::WebInterface(int port, LicenseManagerInterface* license_manager)
    : port_ (port), license_manager_(license_manager)
{
    LOG(DEBUG) << "Webinterface constructor, starting to listen on port " << port_;

    server_.set_access_channels(websocketpp::log::alevel::all);
    server_.clear_access_channels(websocketpp::log::alevel::frame_payload);
    server_.init_asio();
    server_.listen(port_);
    server_.set_open_handler(bind(&WebInterface::OnOpen,this, ::_1));
    server_.set_close_handler(bind(&WebInterface::OnClose,this,::_1));
    server_.set_http_handler(bind(&WebInterface::OnHttp,this,::_1));
    server_.start_accept();
    thread t([this] {
                this->server_.run();
            });
    server_thread_ = move(t);

    LOG(DEBUG) << "Webinterface constructor done successfully";
}

WebInterface::~WebInterface() {
    LOG(DEBUG)<< "Webinterface destructor";

    server_.stop();
    if(server_thread_.joinable()) {
        server_thread_.join();
    }

    LOG(DEBUG)<< "Webinterface destructor done successfully";
}

void WebInterface::OnOpen(connection_hdl hdl) {
    {
        lock_guard<mutex> guard(connection_mutex_);
        LOG(DEBUG)<< "WebInterface onOpen";
        connections_.insert(hdl);
    }
}

void WebInterface::OnClose(connection_hdl hdl) {
    lock_guard<mutex> guard(connection_mutex_);
    LOG(DEBUG)<< "WebInterface onClose";
    connections_.erase(hdl);
}

void WebInterface::OnHttp(connection_hdl hdl) {
    LOG(DEBUG)<< "WebInterface onHttp";
    websocketpp::server<websocketpp::config::asio>::connection_ptr con = server_.get_con_from_hdl(hdl);

    string body = con->get_request_body();
    string uri = con->get_resource();
    string met = con->get_request().get_method();

    HttpResponse response;
    HandleHttpMessage(met,uri,body, response);

    con->set_body(response.response_message_);
    con->set_status((websocketpp::http::status_code::value)response.response_code_);
    if(response.content_type_ != ""){
        con->append_header("content-type", response.content_type_);
    }
}

void WebInterface::HandleHttpMessage(const string& method, const string& path, const string& body, HttpResponse& response) {
    LOG(DEBUG) << "HTTP Message: Method: " << method << "; Path: " << path << "; Body: '" << body << "'.";

    string combined = method + ":" + path + ":" + body;
    boost::smatch what;

    if (boost::regex_search(combined, what, boost::regex("^GET:\\/cmdongles:$"))) {
        HandleGetDongles(response);
    } else if (boost::regex_search(combined, what, boost::regex("^GET:\\/cmdongles\\/([^/]+)\\/context:$"))) {
        HandleGetContext(what[1].str(), response);
    } else if (boost::regex_search(combined, what, boost::regex("^PUT:\\/cmdongles\\/([^/]+)\\/update:.*$"))) {
        HandleUpdate(what[1].str(), body, response);
    } else if (boost::regex_search(combined, what, boost::regex("^GET:\\/cmdongles\\/([^/]+)\\/products\\/([^/]+)\\/licensecount:$"))) {
        HandleGetLicenseCount(what[1].str(), what[2].str(), response);
    } else if(boost::regex_search(combined, what, boost::regex("^.*:\\/cmdongles:.*$"))) {
        response.Set(400, "Wrong method for this URL");
    } else if (boost::regex_search(combined, what, boost::regex("^.*:\\/cmdongles\\/([^/]+)\\/context:.*$"))) {
        response.Set(400, "Wrong method for this URL");
    } else if (boost::regex_search(combined, what, boost::regex("^.*:\\/cmdongles\\/([^/]+)\\/update:.*$"))) {
        response.Set(400, "Wrong method for this URL");
    } else if (boost::regex_search(combined, what, boost::regex("^.*:\\/cmdongles\\/([^/]+)\\/products\\/([^/]+)\\/licensecount:.*$"))) {
        response.Set(400, "Wrong method for this URL");
    } else {
        response.Set(400, "Path could not be recognized");
    }

    if(response.response_code_ == 200){
        LOG(DEBUG) << "HTTP Message successfully handled.";
    } else {
        LOG(DEBUG) << "HTTP Message failed, code: " << response.response_code_ << "; message: '" << response.response_message_ << "'.";
    }
}

void WebInterface::HandleGetDongles(HttpResponse& response){
    LOG(DEBUG)<< "Getting dongles.";
    vector<string> dongles;
    try{
        license_manager_->GetDongles(dongles);
        string dongles_str("[");
        for (auto i : dongles) {
            if(dongles_str != "["){
                dongles_str += ",";
            }
            dongles_str += "\"" + i + "\"";
        }
        dongles_str+="]\n";
        response.Set(200, dongles_str);
        response.SetContentType("application/json");
        LOG(DEBUG)<< "Dongles successfully got.";
    }catch(exception& ex){
        response.Set(400, ex.what());
        LOG(DEBUG) << "Getting dongles failed.";
    }
}

void WebInterface::HandleGetContext(const string& dongle_id, HttpResponse& response){
    LOG(DEBUG) << "Getting context for dongle '" << dongle_id << "'.";
    try{
        string context;
        license_manager_->GetContext(dongle_id, context);
        response.Set(200, context);
        LOG(DEBUG) << "Context successfully got.";
    }catch(exception& ex){
        response.Set(400, ex.what());
        LOG(DEBUG) << "Getting context failed.";
    }
}

void WebInterface::HandleUpdate(const string& dongle_id, const string& rau_data, HttpResponse& response){
    LOG(DEBUG) << "Updating dongle '" << dongle_id << "'.";
    try{
        license_manager_->Update(dongle_id, rau_data);
        response.Set(200, "SUCCESS");
        LOG(DEBUG) << "Dongle successfully updated.";
    }catch(exception& ex){
        response.Set(400, ex.what());
        LOG(DEBUG) << "Updating dongle failed.";
    }
}

void WebInterface::HandleGetLicenseCount(const string& dongle_id, const string& product_id, HttpResponse& response){
    LOG(DEBUG)<< "Getting license count for dongle '" << dongle_id << "' and product id '" << product_id << "'.";
    try{
        size_t count = license_manager_->GetLicenseCount(dongle_id, product_id);
        stringstream ss;
        ss << count;
        response.Set(200, ss.str());
        LOG(DEBUG)<< "License count successfully got.";
    }catch(exception& ex){
        response.Set(400, ex.what());
        LOG(DEBUG) << "Getting license count failed.";
    }
}


