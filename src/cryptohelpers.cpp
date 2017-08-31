#include "cryptohelpers.h"

#include <openssl/evp.h>
#include <openssl/buffer.h>

#include <string.h>

using namespace std;

void CryptoHelpers::Unbase64(const string& in, CryptoBuffer& out){
    unsigned char* buffer = new unsigned char[in.length()+1];
    memset(buffer, 0, in.length()+1);
    BIO* b64 = BIO_new(BIO_f_base64());
    // unfortunately some versions of openssl erroneously lack const at the first arg of the following call, so work around...
    CryptoBuffer workaround;
    workaround.set(in);
    BIO* bmem = BIO_new_mem_buf(workaround, workaround.size());
    bmem = BIO_push(b64, bmem);

    BIO_set_flags(bmem, BIO_FLAGS_BASE64_NO_NL);
    BIO_read(bmem, buffer, in.length());
    out.set(buffer, bmem->num_read);

    BIO_free_all(bmem);
    delete[] buffer;
}

void CryptoHelpers::Base64(const CryptoBuffer& in, std::string& out){
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, in, in.size());
    BIO_flush(bio);
    BUF_MEM *bufferPtr;
    BIO_get_mem_ptr(bio, &bufferPtr);

    out.assign(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
}
