#include "webinterface.h"
#include "easylogging++.h"


#include "httphandler.h"

// using websocketpp::connection_hdl;
// using websocketpp::lib::placeholders::_1;
using namespace std;
using namespace Pistache;


WebInterface::WebInterface(int port)
{
    LOG(DEBUG) << "Webinterface constructor, starting to listen on port " << port;


    Address addr(Pistache::Ipv4::any(), Pistache::Port(port));
    auto opts = Pistache::Http::Endpoint::options()
        .threads(1);
    server_ = std::make_shared<Http::Endpoint>(addr);
    server_->init(opts);
    auto handler = Http::make_handler<HttpHandler>();
    server_->setHandler(handler);
    server_->serve();

    LOG(DEBUG) << "Webinterface constructor done successfully";
}

WebInterface::~WebInterface() {
    LOG(DEBUG)<< "Webinterface destructor";

    server_->shutdown();

    LOG(DEBUG)<< "Webinterface destructor done successfully";
}



