#pragma once

#include "parser.hpp"
#include "module.hpp"

#include <vector>
#include <string>
#include <set>
#include <queue>

namespace helper {
    extern yy::parser::semantic_type* yylval;
    extern std::vector<std::string> errs;
    extern int first_line;  
    extern int last_line;
    extern int first_column;
    extern int last_column;
    extern std::vector<
        std::shared_ptr<mdl::Module>> modules;
    extern std::set<std::string> allModules;
    extern std::queue<std::string> modulesForPars;
    extern std::string curModule;
} // namespace helper