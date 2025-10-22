#pragma once

#include "parser.hpp"

#include <vector>
#include <string>

namespace helper {
    extern yy::parser::semantic_type* yylval;
    extern std::vector<std::string> moduleFileNames;
    extern std::vector<std::string> errs;
    extern bool hasErr;
    extern int lineNo;  
    extern int columnNo;
}