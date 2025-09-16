#include <unistd.h>
#include <fcntl.h>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

#include <FlexLexer.h>

int yyFlexLexer::yywrap() { return 1; }

int main() {
#if defined(TSTDATADIR) && defined(TSTOUTPUTDIR)
    namespace fs = std::filesystem;    
    fs::path tstdir(TSTDATADIR);
    if (!fs::is_directory(tstdir)) {
        std::cerr << "the path is not a directory: " 
                  << TSTDATADIR
                  << '\n';
        return 1;
    }

    for (auto&& entry : fs::directory_iterator(tstdir)) {
        if (entry.is_regular_file()) {
            const auto input = entry.path().string(); 
            const auto output = TSTOUTPUTDIR
                          + fs::path(input).filename().string()
                          + ".output.txt";

            int file = open(output.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
            int d = dup2(file, STDOUT_FILENO);
            close(file);
            if (d == -1) {
                std::cerr << "can't open the output file: "
                          <<  output
                          << '\n';
                continue;
            }
            
            std::ifstream ifs(input);
            if (!ifs.is_open()) {
                std::cerr << "can't open the input file: "
                          << input 
                          << '\n';
                continue;
            }

            yyFlexLexer lexer(&ifs);
            while (lexer.yylex());
        }
    }
    

#else
    static_assert(false &&
        "you need to specify the path to the"
        " folder with .adb files and test output folder.");
#endif // TSTDATADIR
}
