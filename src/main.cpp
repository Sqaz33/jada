#include <iostream>
#include <exception>
#include <fstream>
#include <string_view>
#include <atomic>

#include <FlexLexer.h>

#include "helper.hpp"
#include "parser.hpp"
#include "graphviz.hpp"

namespace helper {
    yy::parser::semantic_type* yylval = nullptr;
    std::vector<std::string> moduleFileNames;
    std::vector<std::string> errs;
    int first_line = 1;
    int last_line = 1;
    int first_column = 1;
    int last_column = 1; 
    std::vector<
        std::shared_ptr<mdl::Module>> modules;
    std::set<std::string> allModules;
    std::queue<std::string> modulesForPars;
    std::mutex compMdlMut;
    std::mutex addMdlNameMut;
    std::mutex addErrMut;
}

namespace {

void printErrors() {
    for (auto&& e : helper::errs) {
        std::cerr << e 
                  << (e.back() == '\n' ? "" : "\n");
    }
}

std::atomic<int> workersWait = 0;
std::atomic<bool> end = false;

void worker() {
    using namespace helper;
    while (!end) {
        std::string mdl;
        {
            std::lock_guard<std::mutex> lk(addMdlNameMut); // TODO: try lock
            if (!modulesForPars.empty()) {
                mdl.swap(modulesForPars.front());
                modulesForPars.pop();
            }
        }

        if (!mdl.empty()) {
            std::ifstream ifs(mdl/*to lower*/+ ".adb");
            if (!ifs.is_open()) {
                std::stringstream ss;
                ss << "Can`t open file ";
                ss << mdl << ".adb";
                ss << " or file doesn't exist";
                throw std::runtime_error(ss.str());
            }
            yyFlexLexer lexer(&ifs);
            yy::parser p(&lexer);
            p.parse();
        }


    }


}

} // namespace

int yyFlexLexer::yywrap() { return 1; }

int main(int argc, char** argv) try {
    if (argc != 2) {
        std::cout << "usage ./jada file.adb" << std::endl;
        return 1;
    }
    // argv = new char*[2];
    // argv[1] = "../test_data/simple.adb";
    
    std::string_view sv(argv[1]);
    if (!sv.ends_with(".adb")) {
        std::cout << "Usage ./jada file.adb" << std::endl;
        return 1;
    } 


    helper::moduleFileNames.push_back(argv[1]);         

    printErrors();
    if (!helper::errs.empty()) {
        return 1;
    }


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