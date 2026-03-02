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
#include "codegen.hpp"
#include "ada_codegen.hpp"

namespace codegen {
    JavaBCCodegen cg(49, 0);
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

        bool anyOpened = false;
        path.replace_filename(mdl + ".adb");
        curModuleFileExtension = "adb";
        std::ifstream ifs(path, std::ios::in);
        if (ifs.is_open()) {
            curModuleName = mdl;
            curModuleFileName = path;
            yyFlexLexer lexer(&ifs);
            yy::parser p(&lexer);
            // p.set_debug_level(1);
            anyOpened = true;
            if(p.parse()) {
                return false;
            }
        } 

        path.replace_filename(mdl + ".ads");
        std::ifstream ifs2(path, std::ios::in);
        curModuleFileExtension = "ads";
        if (ifs2.is_open()) {
            curModuleName = mdl;
            curModuleFileName = path;
            yyFlexLexer lexer(&ifs2);
            yy::parser p(&lexer);
            // p.set_debug_level(1);
            anyOpened = true;
            if(p.parse()) {
                return false;
            }
        }

        if (!anyOpened) {
            std::stringstream ss;
            ss << "Can`t open file ";
            ss << path << "or .adb"; 
            ss << " or file doesn't exist";
            throw std::runtime_error(ss.str());
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
    // линковка выражений и объявлений
    auto LE = std::make_shared<semantics_part::LinkExprs>();
    // проверка типов
    auto TC = std::make_shared<semantics_part::TypeCheck>();
    // расстановка полных квал. имен
    auto QNS = std::make_shared<semantics_part::QualifiedNameSet>();

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
    sem.addPart(LE);
    sem.addPart(TC);
    sem.addPart(QNS);

    auto[ok, msg] = sem.analyse(helper::modules);
    if (!ok) {
        std::cerr << msg << std::endl;
        return 1;
    }
    return 0;
}
// ввод вывод с клавы стринка, инта, флоата, була
void addAdaStdLib(
    std::vector<std::shared_ptr<mdl::Module>>& prog) 
{     
    auto strTy = std::make_shared<node::StringType>();
    auto intTy = std::make_shared<node::SimpleLiteralType>(node::SimpleType::INTEGER);
    auto boolTy = std::make_shared<node::SimpleLiteralType>(node::SimpleType::BOOL);
    auto floatTy = std::make_shared<node::SimpleLiteralType>(node::SimpleType::FLOAT);
    auto charTy = std::make_shared<node::SimpleLiteralType>(node::SimpleType::CHAR);

    strTy->setInf();
    std::vector putLineVars({std::make_shared<node::VarDecl>("str", strTy)});
    putLineVars[0]->setIn(true);
    auto plDecls = std::make_shared<node::DeclArea>();
    auto plBody = std::make_shared<node::Body>();
    auto PutLine = std::make_shared<node::ProcBody>("put_line", putLineVars, plDecls, plBody);
    ////////////////////////// putline ////////////////////////////////////////
    auto desc = PutLine->desc();
    auto plf = codegen::InnerSubprograms->addMethod(PutLine->name(), desc, true);
    auto* plfBB = plf->createBB();
    plf->createAload(plfBB, "str");
    plf->createInvokestatic(plfBB, codegen::AdaUtilityPrintStringBuilder);
    plf->createReturn(plfBB);

    plf->addFlag(codegen::java_bytecode_codegen::AccessFlag::ACC_PUBLIC);
    plf->addFlag(codegen::java_bytecode_codegen::AccessFlag::ACC_STATIC);
    PutLine->setJavaMethod(plf);
    PutLine->setStatic();
    ///////////////////////////////////////////////////////////////////////////


    auto getDecls1 = std::make_shared<node::DeclArea>();
    auto getBody1 = std::make_shared<node::Body>();
    auto inOutInt = std::make_shared<node::VarDecl>("x", intTy);
    inOutInt->setIn(true); 
    inOutInt->setOut(true);
    inOutInt->setParam();
    std::vector getIntVars({inOutInt});
    auto GetInt = std::make_shared<node::ProcBody>("get", getIntVars, getDecls1, getBody1);
    ////////////////////////// GetInt ////////////////////////////////////////
    auto giDesc = GetInt->desc();
    auto gif = codegen::InnerSubprograms->addMethod(GetInt->name(), giDesc, true);
    auto* gifBB = gif->createBB();
    gif->createAload(gifBB, "x");
    gif->createInvokestatic(gifBB, codegen::AdaUtilityReadInt);
    gif->createReturn(gifBB);

    gif->addFlag(codegen::java_bytecode_codegen::AccessFlag::ACC_PUBLIC);
    gif->addFlag(codegen::java_bytecode_codegen::AccessFlag::ACC_STATIC);
    GetInt->setJavaMethod(gif);
    GetInt->setStatic();
    ///////////////////////////////////////////////////////////////////////////


    auto getDecls2 = std::make_shared<node::DeclArea>();
    auto getBody2 = std::make_shared<node::Body>();
    auto inOutBool = std::make_shared<node::VarDecl>("x", boolTy);
    inOutBool->setIn(true); 
    inOutBool->setOut(true);
    inOutBool->setParam();
    std::vector getBoolVars({inOutBool});
    auto GetBool = std::make_shared<node::ProcBody>("get", getBoolVars, getDecls2, getBody2);
    ////////////////////////// GetBool ////////////////////////////////////////
    auto gbDesc = GetBool->desc();
    auto gbf = codegen::InnerSubprograms->addMethod(GetBool->name(), gbDesc, true);
    auto* gbfBB = gbf->createBB();
    gbf->createAload(gbfBB, "x");
    gbf->createInvokestatic(gbfBB, codegen::AdaUtilityReadBool);
    gbf->createReturn(gbfBB);

    gbf->addFlag(codegen::java_bytecode_codegen::AccessFlag::ACC_PUBLIC);
    gbf->addFlag(codegen::java_bytecode_codegen::AccessFlag::ACC_STATIC);
    GetBool->setJavaMethod(gbf);
    GetBool->setStatic();
    ///////////////////////////////////////////////////////////////////////////

    auto getDecls3 = std::make_shared<node::DeclArea>();
    auto getBody3 = std::make_shared<node::Body>();
    auto inOutFloat = std::make_shared<node::VarDecl>("x", floatTy);
    inOutFloat->setIn(true); 
    inOutFloat->setOut(true);
    inOutFloat->setParam();
    std::vector getFloatVars({inOutFloat});
    auto GetFloat = std::make_shared<node::ProcBody>("get", getFloatVars, getDecls3, getBody3);
    ////////////////////////// GetFloat ////////////////////////////////////////
    auto gfDesc = GetFloat->desc();
    auto gff = codegen::InnerSubprograms->addMethod(GetFloat->name(), gfDesc, true);
    auto* gffBB = gff->createBB();
    gff->createAload(gffBB, "x");
    gff->createInvokestatic(gffBB, codegen::AdaUtilityReadFloat);
    gff->createReturn(gffBB);

    gff->addFlag(codegen::java_bytecode_codegen::AccessFlag::ACC_PUBLIC);
    gff->addFlag(codegen::java_bytecode_codegen::AccessFlag::ACC_STATIC);
    GetFloat->setJavaMethod(gff);
    GetFloat->setStatic();
    ///////////////////////////////////////////////////////////////////////////

    auto getDecls4 = std::make_shared<node::DeclArea>();
    auto getBody4 = std::make_shared<node::Body>();
    auto inOutString = std::make_shared<node::VarDecl>("x", strTy);
    inOutString->setIn(true); 
    inOutString->setOut(true);
    inOutString->setParam();
    std::vector getStrVars({inOutString});
    auto GetStr = std::make_shared<node::ProcBody>("get", getStrVars, getDecls4, getBody4);
    ////////////////////////// GetStr ////////////////////////////////////////
    auto gsDesc = GetStr->desc();
    auto gsf = codegen::InnerSubprograms->addMethod(GetStr->name(), gsDesc, true);
    auto* gsfBB = gsf->createBB();
    gsf->createAload(gsfBB, "x");
    gsf->createInvokestatic(gsfBB, codegen::AdaUtilityReadString);
    gsf->createReturn(gsfBB);

    gsf->addFlag(codegen::java_bytecode_codegen::AccessFlag::ACC_PUBLIC);
    gsf->addFlag(codegen::java_bytecode_codegen::AccessFlag::ACC_STATIC);
    GetStr->setJavaMethod(gsf);
    GetStr->setStatic();
    ///////////////////////////////////////////////////////////////////////////


    auto getDecls5 = std::make_shared<node::DeclArea>();
    auto getBody5 = std::make_shared<node::Body>();
    auto inOutChar = std::make_shared<node::VarDecl>("x", charTy);
    inOutChar->setIn(true); 
    inOutChar->setOut(true);
    inOutChar->setParam();
    std::vector getCharVars({inOutChar});
    auto GetChar = std::make_shared<node::ProcBody>("getc", getCharVars, getDecls5, getBody5);
    ////////////////////////// GetChar ////////////////////////////////////////
    auto gcDesc = GetChar->desc();
    auto gcf = codegen::InnerSubprograms->addMethod("getc", gcDesc, true);
    auto* gcfBB = gcf->createBB();
    gcf->createAload(gcfBB, "x");
    gcf->createInvokestatic(gcfBB, codegen::AdaUtilityReadChar);
    gcf->createReturn(gcfBB);

    gcf->addFlag(codegen::java_bytecode_codegen::AccessFlag::ACC_PUBLIC);
    gcf->addFlag(codegen::java_bytecode_codegen::AccessFlag::ACC_STATIC);
    GetChar->setJavaMethod(gcf);
    GetChar->setStatic();
    ///////////////////////////////////////////////////////////////////////////

    auto libAreaTextIO = std::make_shared<node::DeclArea>();
    libAreaTextIO->addDecl(PutLine);
    libAreaTextIO->addDecl(GetInt);
    libAreaTextIO->addDecl(GetBool);
    libAreaTextIO->addDecl(GetFloat);
    libAreaTextIO->addDecl(GetStr);
    libAreaTextIO->addDecl(GetChar);
    auto libUnitTextIO = 
        std::make_shared<node::PackDecl>("text_io", libAreaTextIO);

    auto libArea = std::make_shared<node::DeclArea>();
        libArea->addDecl(libUnitTextIO);

    auto libUnit = 
        std::make_shared<node::PackDecl>("ada", libArea);

    auto mod = std::make_shared<mdl::Module>(
        libUnit, 
        std::vector<std::shared_ptr<node::With>>(), 
        std::vector<std::shared_ptr<node::Use>>(), 
        "ada.text_io", "ada.text_io.ads", "ads");
    helper::modules.push_back(mod);
}
} // namespace

int yyFlexLexer::yywrap() { return 1; }

int main(int argc, char** argv) /*try*/ {
    namespace fs = std::filesystem;

    if (argc == 2 && std::string("-h") == argv[1]) {
        std::cout << 
        R"(Help:
    -h : help
    --pAst-before-semantics : print ast before semantics analysis)" 
        << std::endl;
        return 0;
    }

    if (argc < 2) { // TODO: delete
        argc = 2;
        static std::unique_ptr<char*> argvOwner(new char*[2]);
        argv = argvOwner.get();
        static std::unique_ptr<char> pathOwner;
        char* path = nullptr;
        // argv[1] = "../test_data/complex.adb"; 
        // argv[1] = "../test_data/modules/main.adb"; 
        // argv[1] = "../test_data/semantics/type_replace_check.adb";
        // argv[1] = "../test_data/semantics/in.adb";
        // argv[1] = "../test_data/semantics/record_inherits.adb";
        // argv[1] = "../test_data/semantics/circular/main.adb";
        // argv[1] = "/mnt/d/jada/test_data/semantics/oop1.adb";
        // argv[1] = "../test_data/semantics/return_type.adb";
        // argv[1] = "../test_data/semantics/bool.adb";
        // argv[1] = "../test_data/semantics/inner_package_body_decl.adb";
        // argv[1] = "../test_data/semantics/simple_pack.adb";
        // argv[1] = "../test_data/semantics/pack_private.adb";
        // argv[1] = "../test_data/semantics/pack_linking/main.adb";
        // argv[1] = "/mnt/d/jada/test_data/nesting.adb";
        // argv[1] = "/mnt/d/jada/test_data/semantics/typecheck.adb";
        // path = strdup("/mnt/d/jada/test_data/codegen/out.adb");
        // path = strdup("../test_data/final/multidim.adb");
        path = strdup("../test_data/final/oop.adb");
        // path = strdup("../test_data/codegen/array.adb");
        // path = strdup("/mnt/d/jada/test_data/overload.adb");
        // path = strdup("/mnt/d/jada/test_data/complex.adb");
        // path = strdup("/mnt/d/jada/test_data/codegen/array.adb");
        // path = strdup("/mnt/d/jada/test_data/codegen/pack.adb");
        // path = strdup("/mnt/d/jada/test_data/codegen/oop.adb");
        // path = strdup("/mnt/d/jada/test_data/codegen/sort.adb");
        // path = strdup("/mnt/d/jada/test_data/test.adb");
        // path = strdup("/mnt/d/jada/test_data/codegen/record.adb");
        // path = strdup("/mnt/d/jada/test_data/codegen/call.adb");
        // path = strdup("/mnt/d/jada/test_data/codegen/ref.adb");
        // path = strdup("/mnt/d/jada/test_data/codegen/bool.adb");
        // path = strdup("/mnt/d/jada/test_data/codegen/loop.adb");
        // path = strdup("/mnt/d/jada/test_data/codegen/branch.adb");
        // argv[1] = "/mnt/d/jada/test_data/semantics/pack_linking/main.adb";
        // argv[1] = "/mnt/d/jada/test_data/semantics/bool.adb";
        // argv[1] = "/mnt/d/jada/test_data/semantics/for_linking.adb";
        // argv[1] = "/mnt/d/jada/test_data/test.adb";
        pathOwner.reset(path);
        argv[1] = pathOwner.get();
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

    codegen::initAdaUtilityNames();
    addAdaStdLib(helper::modules);

    if (!parseProgram(path.remove_filename())) {
        printErrors();
        return 1;
    }
        // TODO: delete
    if (/* true || */ 3 == argc && 
        std::string("--pAst-before-semantics") == argv[2]) 
    {
        printAst();
    }

    if (!helper::rightEnding) {
        std::cerr << "The declaration has" 
                     " different names and endings\n";
        return 1;
    }

    int res = semanticAnalysis();
    if (res != 0)  return res;
    // if (res == 0) {
    //     std::cout << "semantic analysis: OK\n"; // TODO: delete
    // } else {
    //     return res;
    // }

    codegen::gen(helper::modules);

    return 0;
}
// } catch (const std::exception& e) { // TODO 
//     std::cerr << e.what() << '\n';
//     printErrors();
//     return 1;
// }
// } catch (...) {
//     std::cerr << "Unknown error\n";
//     printErrors();
//     return 1;
// }