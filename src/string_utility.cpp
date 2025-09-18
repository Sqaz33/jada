#include "string_utility.hpp"

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

} // namespace utility