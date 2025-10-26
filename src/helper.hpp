#pragma once

#include "parser.hpp"

#include <vector>
#include <string>

namespace helper {
    extern yy::parser::semantic_type* yylval;
    extern std::vector<std::string> moduleFileNames;
    extern std::vector<std::string> errs;
    extern int first_line;  
    extern int last_line;
    extern int first_column;
    extern int last_column;
}