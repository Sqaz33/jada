#include "string_utility.hpp"

#include <algorithm>

namespace utility {

void replaceAll(std::string& str, 
                 const std::string& src, 
                 const std::string& dst) 
{
    auto pos = str.find(src);
    while (pos != str.npos) {
        str.replace(pos, src.length(), dst);
        pos = str.find(src, pos+1);
    } 
}

void toLower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [] (char c) { return std::tolower(c); }
    );
}

std::string toLower(const std::string& str, bool) {
    auto cp = str;
    toLower(cp);
    return cp;
}

} // namespace utility