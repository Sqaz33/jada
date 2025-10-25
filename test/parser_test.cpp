#include <unistd.h>
#include <fcntl.h>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

#include <FlexLexer.h>

#include "helper.hpp"
#include "parser.hpp"

namespace helper {
    yy::parser::semantic_type* yylval = nullptr;
    std::vector<std::string> moduleFileNames;
    std::vector<std::string> errs;
    bool hasErr = false;
    int lineNo = 0;
    int columnNo = 0;
}

int yyFlexLexer::yywrap() { return 1; }

int main() {
#if defined(TSTDATADIR) && defined(TSTOUTPUTDIR)
    namespace fs = std::filesystem;    
    fs::path tstdir(TSTDATADIR);
    if (!fs::is_directory(tstdir)) {
        std::cout << "the path is not a directory: " 
                  << TSTDATADIR
                  << '\n';
        return 1;
    }
    helper::moduleFileNames.push_back("test");
    int savedStdOut = dup(STDERR_FILENO);
    for (auto&& entry : fs::directory_iterator(tstdir)) {
        if (entry.is_regular_file()) {
            const auto input = entry.path().string(); 
            const auto output = TSTOUTPUTDIR
                          + fs::path(input).filename().string()
                          + ".output.txt";

            int file = open(output.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
            int d = dup2(file, STDERR_FILENO);
            close(file);
            if (d == -1) {
                std::cout << "can't open the output file: "
                          <<  output
                          << '\n';
                continue;
            }
            
            std::ifstream ifs(input);
            if (!ifs.is_open()) {
                std::cout << "can't open the input file: "
                          << input 
                          << '\n';
                continue;
            }
            
            yyFlexLexer lexer(&ifs);
            yy::parser p(&lexer);
            p.set_debug_level(1);
            
            if (p.parse()) {
                dup2(savedStdOut, STDERR_FILENO);
                std::cout << "Error on " << input << '\n';
            }
        }
    }
    

#else
    static_assert(false &&
        "you need to specify the path to the"
        " folder with .adb files and test output folder.");
#endif // TSTDATADIR
}
