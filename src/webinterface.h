#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H


#include <thread>
#include <mutex>
#include <set>


#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

class LicenseManagerInterface;

class WebInterface{

    public:

        WebInterface(int port);
        virtual ~WebInterface();

    private:
       
        std::shared_ptr<Pistache::Http::Endpoint> server_;

        
};


#endif
