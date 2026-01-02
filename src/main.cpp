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
#include "semantics.hpp"
#include "semantics_part.hpp"

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
        path.replace_filename(mdl + ".adb");

        std::ifstream ifs(path, std::ios::in);

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
    auto gv = 
        graphviz::createGraphViz(true, false, "ast");
    auto root = gv->addVertex("Program");
    for (auto m : helper::modules) {
        m->print(*gv, root);
    }
    gv->printDOT(std::cout);
}

int semanticAnalysis() {
    semantics::ADASementics sem;
    auto EPC = 
        std::make_shared<semantics_part::EntryPointCheck>();
    auto MNC = 
        std::make_shared<semantics_part::ModuleNameCheck>();
    sem.addPart(EPC);
    sem.addPart(MNC);
    auto[ok, msg] = sem.analyse(helper::modules);
    if (!ok) {
        std::cerr << msg << std::endl;
        return 1;
    }
    return 0;
}

} // namespace

int yyFlexLexer::yywrap() { return 1; }

int main(int argc, char** argv) try {
    namespace fs = std::filesystem;

    if (argc == 2 && std::string("-h") == argv[1]) {
        std::cout << 
        R"(Help:
    -h : help
    --pAst-before-semantics : print ast before semantics analysis)";
        return 0;
    }

    if (argc < 2) {
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
    
    auto mdl = path.filename();
    mdl.replace_extension("");
    helper::modulesForPars.push(mdl.string());

    helper::allModules.insert("ada");

    if (!parseProgram(path.remove_filename())) {
        printErrors();
        return 1;
    }

    if (3 == argc && 
        std::string("--pAst-before-semantics") == argv[2]) 
    {
        printAst();
    }

    return semanticAnalysis();
} catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    printErrors();
    return 1;

} catch (...) {
    std::cerr << "Unknown error\n";
    printErrors();
    return 1;
}