#ifndef SRC_CRYPTOHELPERS_H_
#define SRC_CRYPTOHELPERS_H_

#include "cryptobuffer.h"
#include <string>

class CryptoHelpers {
    public:
        static void Unbase64(const std::string& in, CryptoBuffer& out);
        static void Base64(const CryptoBuffer& in, std::string& out);

    private:
        CryptoHelpers(){}
        ~CryptoHelpers(){}
};

#endif
