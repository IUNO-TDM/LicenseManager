#include "licensemanager.h"
#include "webinterface.h"

#include "easylogging++.h"

#include <boost/program_options.hpp>

using namespace std;
using namespace boost::program_options;

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[]) {
    LOG(INFO)<< "LicenseManager starting up.";

    int websocket_port = 11432;

    LicenseManager license_manager;
    WebInterface web_interface(websocket_port, &license_manager);

    cin.get();

    LOG(INFO)<< "LicenseManager exiting normally.";
    el::Loggers::flushAll();
    return 0;
}
