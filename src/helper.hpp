#pragma once

#include "parser.hpp"

#include <vector>
#include <string>

namespace helper {
    extern yy::parser::semantic_type* yylval;
    extern std::vector<std::string> modules;
    extern std::vector<std::string> errs;
    extern std::string curModule;
    extern bool hasErr;
    extern int lineNo;  
    extern int columnNo;
}