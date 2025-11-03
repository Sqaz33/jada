#pragma once

#include "parser.hpp"

#include <vector>
#include <string>
#include <mutex>
#include <set>
#include <queue>

namespace helper {
    extern yy::parser::semantic_type* yylval;
    extern std::vector<std::string> moduleFileNames;
    extern std::vector<std::string> errs;
    extern int first_line;  
    extern int last_line;
    extern int first_column;
    extern int last_column;
    extern std::vector<
        std::shared_ptr<mdl::Module>> modules;
    extern std::set<std::string> allModules;
    extern std::queue<std::string> modulesForPars;
    extern std::mutex compMdlMut;
    extern std::mutex addMdlNameMut;
    extern std::mutex addErrMut;
} // namespace helper