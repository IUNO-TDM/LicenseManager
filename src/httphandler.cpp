#include "httphandler.h"
#include "webinterface.h"
#include <sstream>

using namespace std;
using namespace Pistache;

WebInterface* HttpHandler::WebInterface_ = NULL;

void HttpHandler::SetWebInterface(WebInterface* wi){
    if( WebInterface_ && wi){
        throw logic_error("Tried to SetWebInterface twice");
    }
    WebInterface_ = wi;
}

void HttpHandler::onRequest(const Http::Request& request, Http::ResponseWriter response){
    WebInterface::HttpResponse resp;
    WebInterface_->HandleHttpMessage(Http::methodString(request.method()),request.resource(),request.body(), resp);
    response.send(Http::Code(resp.response_code_), resp.response_message_);
}


