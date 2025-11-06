#include <iostream>
#include <exception>
#include <fstream>
#include <string_view>
#include <algorithm>
#include <filesystem>

#include <FlexLexer.h>

#include "helper.hpp"
#include "parser.hpp"
#include "graphviz.hpp"
#include "string_utility.hpp"
#include "graphviz.hpp"

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
}

namespace {

void printErrors() {
    for (auto&& e : helper::errs) {
        std::cerr << e 
                  << (e.back() == '\n' ? "" : "\n");
    }
}

bool parseProgram(std::filesystem::path path) {
    using namespace helper;

    while (!modulesForPars.empty()) {
        std::string mdl = modulesForPars.front();
        modulesForPars.pop();
        utility::toLower(mdl);
        path.replace_filename(mdl);
        path.replace_extension(".adb");

        std::ifstream ifs(path);

        if (!ifs.is_open()) {
            std::stringstream ss;
            ss << "Can`t open file ";
            ss << path;
            ss << " or file doesn't exist";
            throw std::runtime_error(ss.str());
        }

        curModuleName = mdl;
        curModuleFileName = path;

        yyFlexLexer lexer(&ifs);
        yy::parser p(&lexer);
        if(p.parse()) {
            return false;
        }
    }
    return true;
}

void printAst() {
    graphviz::GraphViz gv(true, false, "ast");
    auto root = gv.addVertex("Program");
    for (auto m : helper::modules) {
        m->print(gv, root);
    }
    gv.printDOT(std::cout);
}

} // namespace

int yyFlexLexer::yywrap() { return 1; }

int main(int argc, char** argv) try {
    namespace fs = std::filesystem;

    if (argc != 2) {
        std::cout << "usage ./jada file.adb" << std::endl;
        return 1;
    }

    // argv = new char*[2]; // TODO: delete
    // argv[1] = "../test_data/modules/main.adb"; // TODO: delete
    
    fs::path path(argv[1]);
    if (".adb" != path.extension()) {
        std::cout << "Usage ./jada file.adb" << std::endl;
        return 1;
    } 
    path.replace_extension("");
    helper::modulesForPars.push(path.filename());

    if (!parseProgram(path.remove_filename())) {
        printErrors();
        return 1;
    }

    printAst();

    return 0;
} catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    printErrors();
    return 1;

} catch (...) {
    std::cerr << "Unknown error\n";
    printErrors();
    return 1;
}