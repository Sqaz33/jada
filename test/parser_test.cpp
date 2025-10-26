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
    int first_line = 1;
    int last_line = 1;
    int first_column = 1;
    int last_column = 1;
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
    int savedStdOut = dup(STDERR_FILENO);
    for (auto&& entry : fs::directory_iterator(tstdir)) {
        if (entry.is_regular_file()) {
            const auto input = entry.path().string(); 
            const auto output = TSTOUTPUTDIR
                          + fs::path(input).filename().string()
                          + ".output.txt";

            int file = open(output.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
            helper::moduleFileNames.push_back(input.c_str());
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
                std::cout << "Error on " << input << '\n';
                dup2(savedStdOut, STDERR_FILENO);
                for (auto&& e : helper::errs) {
                    std::cerr << e << '\n';
                }
            } else {
                std::cout << "Done: " << input << '\n';
            }
            helper::errs.clear();
            helper::moduleFileNames.clear();
        }
    }
    

#else
    static_assert(false &&
        "you need to specify the path to the"
        " folder with .adb files and test output folder.");
#endif // TSTDATADIR
}
