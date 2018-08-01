#include "regexwrapper.h"

#include <sys/types.h>
#include <regex.h>

using namespace std;

struct RegexWrapper::RegexComp: public regex_t{
};

struct RegexWrapper::RegexMatch: public regmatch_t{
};

RegexWrapper::RegexWrapper(const char* regex){

    regex_comp_ = new RegexComp;
    regcomp(regex_comp_, regex, REG_EXTENDED);

    const char* p = regex;
    nmatch_ = 1; // first match is the complete match
    while(*p){
        if(*p=='('){
            nmatch_++;
        }
        p++;
    }

    pmatch_ = new RegexMatch[nmatch_];
}

RegexWrapper::~RegexWrapper(){
    delete[] pmatch_;

    regfree(regex_comp_);
    delete regex_comp_;
}

bool RegexWrapper::match(const string& match_str){
    last_match_str_=match_str;
    return match(match_str.c_str());
}

bool RegexWrapper::match(const char* match_str){
    return 0 == regexec(regex_comp_, match_str, nmatch_, pmatch_, 0);
}

string RegexWrapper::extractMatch(size_t nr) const {
    size_t start = pmatch_[nr].rm_so;
    size_t len = pmatch_[nr].rm_eo - start;
    return last_match_str_.substr(start, len);
}
