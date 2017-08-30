#include "licensemanager.h"
#include "webinterface.h"

#include "easylogging++.h"

#include <boost/program_options.hpp>
#include <signal.h>

using namespace std;
using namespace boost::program_options;

INITIALIZE_EASYLOGGINGPP

bool sig_term_got = false;

void sigTermHandler( int ) {
    sig_term_got = true;
}

int main(int argc, char* argv[]) {
    LOG(INFO)<< "LicenseManager starting up.";

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGTERM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigTermHandler;
    struct sigaction so;
    memset(&so, 0, sizeof(so));

    bool sigterm_installed = (0 == sigaction(SIGTERM, &sa, &so));
    if(!sigterm_installed){
        LOG(WARNING) << "Could not install terminate handler.";
    }

    {
        int websocket_port = 11432;

        LicenseManager license_manager;
        WebInterface web_interface(websocket_port, &license_manager);

        while(!sig_term_got){
            sleep(0xffffffff);
        }
    }

    if(sigterm_installed){
        sigaction(SIGTERM, &so, NULL);
    }

    LOG(INFO)<< "LicenseManager exiting normally.";
    el::Loggers::flushAll();
    return 0;
}
