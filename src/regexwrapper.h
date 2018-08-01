#ifndef SRC_REGEXWRAPPER_H_
#define SRC_REGEXWRAPPER_H_

#include <string>

class RegexWrapper {
    struct RegexComp;
    struct RegexMatch;
    public:
        RegexWrapper(const char* regex);
        ~RegexWrapper();

        bool match(const char* match_str);
        bool match(const std::string& match_str);
        std::string extractMatch(size_t nr) const;

    private:
        RegexComp* regex_comp_;
        size_t nmatch_;
        RegexMatch* pmatch_;
        std::string last_match_str_;
};

#endif
