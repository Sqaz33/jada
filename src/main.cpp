#include <iostream>
#include <exception>
#include <fstream>
#include <string_view>

#include <FlexLexer.h>

#include "helper.hpp"
#include "parser.hpp"

namespace helper {
    yy::parser::semantic_type* yylval = nullptr;
    std::vector<std::string> moduleFileNames;
    std::vector<std::string> errs;
    int first_line = 1;
    int last_line = 1;
    int first_column = 1;
    int last_column = 1; 
}

static void printErrors() {
    for (auto&& e : helper::errs) {
        std::cerr << e 
                  << (e.back() == '\n' ? "" : "\n");
    }
}

int yyFlexLexer::yywrap() { return 1; }

int main(int argc, char** argv) try {
    if (argc != 2) {
        std::cout << "usage ./jada file.adb" << std::endl;
        return 1;
    }
    
    std::string_view sv(argv[1]);
    if (!sv.ends_with(".adb")) {
        if (sv.ends_with(".ads")) {
            std::cout << "ads file format" 
                         "is not available yet" 
                      << std::endl;
            return 1;
        }
        std::cout << "usage ./jada file.adb" << std::endl;
        return 1;
    } 

    std::ifstream ifs(argv[1]);
    if (!ifs.is_open()) {
        std::cout << "Can`t open file\n";
        return 1; 
    }
    helper::moduleFileNames.push_back(argv[1]);         


    yyFlexLexer lexer(&ifs);
    yy::parser p(&lexer);
    p.set_debug_level(1);
    
    int res = !p.parse();

    printErrors();

    return res;

} catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    printErrors();

    return 1;
} catch (...) {
    std::cerr << "Unknown error\n";
    printErrors();
    return 1;
}