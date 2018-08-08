#ifndef HTTPHANLDER_H
#define HTTPHANLDER_H

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

class WebInterface;

class HttpHandler : public Pistache::Http::Handler {

    public:
        HTTP_PROTOTYPE(HttpHandler)

        HttpHandler(){}
        virtual ~HttpHandler(){}
        void onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response);

        static void SetWebInterface(WebInterface* wi);

    private:

        static WebInterface* WebInterface_;
};

#endif
