#pragma once

#include <string>

namespace utility {

void replaceAll(std::string& str, 
                 const std::string& src, 
                 const std::string& dst);

void toLower(std::string& str);

std::string toLower(const std::string& str, bool);

} // namespace utility