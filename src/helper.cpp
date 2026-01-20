#include "helper.hpp"

namespace helper {
    yy::parser::semantic_type* yylval = nullptr;
    std::vector<std::string> errs;
    int first_line = 1;
    int last_line = 1;
    int first_column = 1;
    int last_column = 1; 
    std::vector<
        std::shared_ptr<mdl::Module>> modules;
    std::set<std::string> allModules;
    std::queue<std::string> modulesForPars;
    std::string curModuleFileName;
    std::string curModuleName;
    bool rightEnding = true;
}