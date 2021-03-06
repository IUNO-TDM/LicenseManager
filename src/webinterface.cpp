#include "webinterface.h"
#include "easylogging++.h"

#include "webinterface.h"
#include "httphandler.h"
#include "licensemanagerinterface.h"

using namespace std;
using namespace Pistache;


WebInterface::WebInterface(int port, LicenseManagerInterface* lm):
    rxHandleGetDongles_("^GET:\\/cmdongles:$"),
    rxHandleGetContext_("^GET:\\/cmdongles\\/([^/]+)\\/context:$"),
    rxHandleUpdate_("^PUT:\\/cmdongles\\/([^/]+)\\/update:.*$"),
    rxHandleGetLicenseCount_("^GET:\\/cmdongles\\/([^/]+)\\/products\\/([^/]+)\\/licensecount:$"),
    rxHandleGetLicenses_("^GET:\\/cmdongles\\/([^/]+)\\/licenses:$"),
    rxHandleGetDongles405_("^.*:\\/cmdongles:.*$"),
    rxHandleGetContext405_("^.*:\\/cmdongles\\/([^/]+)\\/context:.*$"),
    rxHandleUpdate405_("^.*:\\/cmdongles\\/([^/]+)\\/update:.*$"),
    rxHandleGetLicenseCount405_("^.*:\\/cmdongles\\/([^/]+)\\/products\\/([^/]+)\\/licensecount:.*$"),
    rxHandleGetLicenses405_("^.*:\\/cmdongles\\/([^/]+)\\/licenses:$"),
    license_manager_(lm),
    server_(NULL)
{
    LOG(DEBUG) << "Webinterface constructor, starting to listen on port " << port;

    HttpHandler::SetWebInterface(this);

    Address addr(Pistache::Ipv4::any(), Pistache::Port(port));
    auto opts = Pistache::Http::Endpoint::options().threads(1);
    server_ = new Http::Endpoint(addr);
    server_->init(opts);
    shared_ptr<HttpHandler> handler(new HttpHandler);
    server_->setHandler(handler);
    server_->serveThreaded();

    LOG(DEBUG) << "Webinterface constructor done successfully";
}

WebInterface::~WebInterface() {
    LOG(DEBUG)<< "Webinterface destructor";

    server_->shutdown();
    delete server_;

    HttpHandler::SetWebInterface(NULL);

    LOG(DEBUG)<< "Webinterface destructor done successfully";
}

void WebInterface::HandleHttpMessage(const string& method, const string& path, const string& body, HttpResponse& response) {
    LOG(DEBUG) << "HTTP Message: Method: " << method << "; Path: " << path << "; Body: '" << body << "'.";

    string combined = method + ":" + path + ":" + body;

    if (rxHandleGetDongles_.match(combined)) {
        HandleGetDongles(response);
    } else if (rxHandleGetContext_.match(combined)) {
        HandleGetContext(rxHandleGetContext_.extractMatch(1), response);
    } else if (rxHandleUpdate_.match(combined)) {
        HandleUpdate(rxHandleUpdate_.extractMatch(1), body, response);
    } else if (rxHandleGetLicenseCount_.match(combined)) {
        HandleGetLicenseCount(rxHandleGetLicenseCount_.extractMatch(1), rxHandleGetLicenseCount_.extractMatch(2), response);
    } else if (rxHandleGetLicenses_.match(combined)) {
        HandleGetLicenses(rxHandleGetLicenses_.extractMatch(1), response);
    } else if(rxHandleGetDongles405_.match(combined)) {
        response.Set(405, "Wrong method for this URL");
    } else if (rxHandleGetContext405_.match(combined)) {
        response.Set(405, "Wrong method for this URL");
    } else if (rxHandleUpdate405_.match(combined)) {
        response.Set(405, "Wrong method for this URL");
    } else if (rxHandleGetLicenseCount405_.match(combined)) {
        response.Set(405, "Wrong method for this URL");
    } else if (rxHandleGetLicenses405_.match(combined)) {
        response.Set(405, "Wrong method for this URL");
    } else {
        response.Set(404, "Path could not be recognized");
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
        response.Set(500, ex.what());
        LOG(DEBUG) << "Getting dongles failed. Reason: '" << ex.what() << "'.";
    }
}

void WebInterface::HandleGetContext(const string& dongle_id, HttpResponse& response){
    LOG(DEBUG) << "Getting context for dongle '" << dongle_id << "'.";
    try{
        string context;
        license_manager_->GetContext(dongle_id, context);
        response.Set(200, context);
        LOG(DEBUG) << "Context successfully got.";
    }catch(invalid_argument& ex){
        response.Set(404, ex.what());
        LOG(DEBUG) << "Getting context failed. Reason: '" << ex.what() << "'.";
    }catch(exception& ex){
        response.Set(500, ex.what());
        LOG(DEBUG) << "Getting context failed. Reason: '" << ex.what() << "'.";
    }
}

void WebInterface::HandleUpdate(const string& dongle_id, const string& rau_data, HttpResponse& response){
    LOG(DEBUG) << "Updating dongle '" << dongle_id << "'.";
    try{
        license_manager_->Update(dongle_id, rau_data);
        response.Set(200, "SUCCESS");
        LOG(DEBUG) << "Dongle successfully updated.";
    }catch(invalid_argument& ex){
        response.Set(404, ex.what());
        LOG(DEBUG) << "Updating dongle failed. Reason: '" << ex.what() << "'.";
    }catch(runtime_error& ex){
        response.Set(422, ex.what());
        LOG(DEBUG) << "Updating dongle failed. Reason: '" << ex.what() << "'.";
    }catch(exception& ex){
        response.Set(500, ex.what());
        LOG(DEBUG) << "Updating dongle failed. Reason: '" << ex.what() << "'.";
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
    }catch(invalid_argument& ex){
        response.Set(404, ex.what());
        LOG(DEBUG) << "Getting license count failed. Reason: '" << ex.what() << "'.";
    }catch(exception& ex){
        response.Set(500, ex.what());
        LOG(DEBUG) << "Getting license count failed. Reason: '" << ex.what() << "'.";
    }
}

void WebInterface::HandleGetLicenses(const string& dongle_id, HttpResponse& response){
    LOG(DEBUG)<< "Getting license count for dongle '" << dongle_id << "'.";
    try{
        map<unsigned long, unsigned long> licenses;
        license_manager_->GetLicenses(dongle_id, licenses);
        stringstream r;
        r << "[";
        bool is_first = true;
        for (auto& i : licenses){
            r << (is_first?"":", ") << "{\"product\":\"" << i.first << "\", \"count\":" << i.second << "}";
            is_first=false;
        }
        r << "]";
        response.Set(200, r.str());
        LOG(DEBUG)<< "Licenses successfully got.";
    }catch(invalid_argument& ex){
        response.Set(404, ex.what());
        LOG(DEBUG) << "Getting licenses failed. Reason: '" << ex.what() << "'.";
    }catch(exception& ex){
        response.Set(500, ex.what());
        LOG(DEBUG) << "Getting licenses failed. Reason: '" << ex.what() << "'.";
    }
}


