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
    auto EPC = // проверка на точку входа - процедуру
        std::make_shared<semantics_part::EntryPointCheck>();
    auto MNC = // проверка на соотв. имени файла и имени ед. комп.
        std::make_shared<semantics_part::ModuleNameCheck>();
    auto OLC  = // --- не нужно ---
        std::make_shared<semantics_part::OneLevelWithCheck>();
    auto SIC = // провекра на импорт самого себя
        std::make_shared<semantics_part::SelfImportCheck>();
    auto EMIC = // проверка на импорт существ. модуля 
        std::make_shared<semantics_part::ExistingModuleImportCheck>();
    auto GSC = // создание верхнего спейса в каждом модуле, занесение в него импортов
        std::make_shared<semantics_part::GlobalSpaceCreation>();
    auto CIC = // проверка импорта друг-друга
        std::make_shared<semantics_part::CircularImportCheck>();
    auto NCC = // проверка конфликта имен в одном спейсе
        std::make_shared<semantics_part::NameConflictCheck>();
    auto PBDL = // линковка боди и декла пакета
        std::make_shared<semantics_part::PackBodyNDeclLinking>();
    auto TNRT = // замена имени типа на указатель на реальную структуру из дерева
        std::make_shared<semantics_part::TypeNameToRealType>();
    auto IVNCC = // проверка переопределения перменной в наслед. рекорде
        std::make_shared<semantics_part::InheritsVarNameConlflicCheck>();
    auto OC = // проверка перегрузки (разная для ф-ций и процедур)
        std::make_shared<semantics_part::OverloadCheck>();
    auto SBDL // линковка декла и боди подпрогр. и проверка на неопределенные боди для деклов 
        = std::make_shared<semantics_part::SubprogBodyNDeclLinking>();
    auto CCD = // создания ооп класса из tagged типа и подпрог. в пакете
        std::make_shared<semantics_part::CreateClassDeclaration>();
    auto OCSC = // проверка на наличие только одного типа ооп класса в параметрах подпрогр. 
        std::make_shared<semantics_part::OneClassInSubprogramCheck>();

    sem.addPart(EPC);
    sem.addPart(MNC);
    sem.addPart(OLC);
    sem.addPart(SIC);
    sem.addPart(EMIC);
    sem.addPart(GSC);
    sem.addPart(CIC);
    sem.addPart(NCC);
    sem.addPart(PBDL);
    sem.addPart(TNRT);
    sem.addPart(IVNCC);
    sem.addPart(OC);
    sem.addPart(SBDL);
    sem.addPart(CCD);
    sem.addPart(OCSC);

    auto[ok, msg] = sem.analyse(helper::modules);
    if (!ok) {
        std::cerr << msg << std::endl;
        return 1;
    }
    return 0;
}

void addAdaStdLib(
    std::vector<std::shared_ptr<mdl::Module>>& prog) 
{   
    auto libArea = std::make_shared<node::DeclArea>();
    // libArea->addDecl(std::make_shared<std::decl>)
    auto libUnit = 
        std::make_shared<node::PackDecl>("ada.text_io", libArea);

    auto mod = std::make_shared<mdl::Module>(
        libUnit, 
        std::vector<std::shared_ptr<node::With>>(), 
        std::vector<std::shared_ptr<node::Use>>(), 
        "ada.text_io", "ada.text_io.adb");
    helper::modules.push_back(mod);
}

} // namespace

int yyFlexLexer::yywrap() { return 1; }

int main(int argc, char** argv) { // try {
    namespace fs = std::filesystem;

    if (argc == 2 && std::string("-h") == argv[1]) {
        std::cout << 
        R"(Help:
    -h : help
    --pAst-before-semantics : print ast before semantics analysis)";
        return 0;
    }

    if (argc < 2) { // TODO: delete
        argc = 2;
        argv = new char*[2]; // TODO: delete
        // argv[1] = "../test_data/complex.adb"; // TODO: delete
        // argv[1] = "../test_data/modules/main.adb"; // TODO: delete
        // argv[1] = "../test_data/semantics/type_replace_check.adb";
        // argv[1] = "../test_data/semantics/proc_func_overload.adb";
        // argv[1] = "../test_data/semantics/record_inherits.adb";
        // argv[1] = "../test_data/semantics/circular/main.adb";
        // argv[1] = "../test_data/semantics/oop1.adb";
        // argv[1] = "../test_data/semantics/return_type.adb";
        argv[1] = "../test_data/semantics/bool.adb";
    }
    
    if (argc < 2) {
        std::cout << "usage ./jada file.adb" << std::endl;
        return 1;
    }

    fs::path path(argv[1]);
    if (".adb" != path.extension()) {
        std::cout << "Usage ./jada file.adb; -h for help" << std::endl;
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
        // TODO: delete
    if (true || 3 == argc && 
        std::string("--pAst-before-semantics") == argv[2]) 
    {
        printAst();
    }

    if (!helper::rightEnding) {
        std::cerr << "The declaration has" 
                     " different names and endings\n";
        return 1;
    }

    addAdaStdLib(helper::modules);

    return semanticAnalysis();
} // catch (const std::exception& e) { // TODO 
//     std::cerr << e.what() << '\n';
//     printErrors();
//     return 1;

// } catch (...) {
//     std::cerr << "Unknown error\n";
//     printErrors();
//     return 1;
// }