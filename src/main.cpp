#include <iostream>
#include <exception>
#include <fstream>
#include <string_view>

#include "FlexLexer.h"

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
    yyFlexLexer lexer(&ifs);
    while (lexer.yylex());

    return 0;

} catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return 1;
} catch (...) {
    std::cerr << "Unknown error\n";
    return 1;
}