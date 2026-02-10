#include "semantics_part.hpp"

#include <sstream>
#include <ranges>
#include <set>
#include <iterator>
#include <algorithm>

#include "node.hpp"
#include "string_utility.hpp"

namespace semantics_part {

// EntryPointCheck
std::string EntryPointCheck::analyse(
        const std::vector<
                std::shared_ptr<mdl::Module>>& program) 
{   
    auto unit = program[0]->unit().lock();
    if (!std::dynamic_pointer_cast<node::ProcBody>(unit) ||
         std::dynamic_pointer_cast<node::FuncBody>(unit)) 
    {
        return program[0]->fileName()
                + ": The entry point should be a procedure";
    }
    return ISemanticsPart::analyseNext(program);
}

// ModuleNameCheck
std::string ModuleNameCheck::analyse(
        const std::vector<
                std::shared_ptr<mdl::Module>>& program) 
{   
    for (auto&& mod : program | std::views::drop(1)) {
        auto unit = mod->unit().lock();
        if (unit->name() != mod->name()) {
            std::stringstream ss;
            ss << mod->fileName();
            ss << ":";
            ss << " The name of the compilation unit must"
                  " be equal to the name of the code file: ";
            ss << mod->name();
            ss << " != ";
            ss << unit->name();
            return ss.str();
        }

        bool packDecl = 
            std::dynamic_pointer_cast<node::PackDecl>(unit) && 
            !std::dynamic_pointer_cast<node::PackBody>(unit);
        bool isAds = mod->fileExtension() == "ads";
        if (isAds ^ packDecl) {
            std::stringstream ss;
            ss << mod->fileName();
            ss << ":";
            ss << "Files with the ads extension must" 
                  " contain the package declaration";
            return ss.str();
        }
    }

    return ISemanticsPart::analyseNext(program);
}

// OneLevelWithCheck 
std::string OneLevelWithCheck::analyse(
        const std::vector<
                std::shared_ptr<mdl::Module>>& program) 
{   
    for (auto&& mod : program) {
        for (auto&& with : mod->with()) {
            auto&& name = with->name();
            if (name.size() != 1 && 
                utility::toLower(name.first(), bool()) != "ada") 
            {
                std::stringstream ss;
                ss << mod->fileName();
                ss << ":";
                ss << "The With statement must contain one name level.";
                return ss.str();
            }
        }
    }

    return ISemanticsPart::analyseNext(program);
}

// SelfImportCheck
std::string SelfImportCheck::analyse(
        const std::vector<
                std::shared_ptr<mdl::Module>>& program) 
{   
    for (auto&& mod : program) {
        for (auto&& with : mod->with()) {
            auto&& name = with->name();
            if (name.first() == mod->name()) {
                std::stringstream ss;
                ss << mod->fileName();
                ss << ":";
                ss << "The module cannot import itself.";
                return ss.str();
            }
        }
    }

    return ISemanticsPart::analyseNext(program);
}

// ExistingModuleImportCheck
std::string ExistingModuleImportCheck::analyse(
        const std::vector<
                std::shared_ptr<mdl::Module>>& program) 
{   
    std::set<std::string> moduleNames;
    std::transform(program.begin(), program.end(), 
                   std::inserter(moduleNames, moduleNames.end()), 
                   [] (auto&& mod) { return mod->name(); });

    for (auto&& mod : program) {
        for (auto&& with : mod->with()) {
            auto&& name = with->name().toString('.');
            if (!moduleNames.contains(name)) {
                std::stringstream ss;
                ss << mod->fileName();
                ss << ":";
                ss << "Importing a non-existing module";
                return ss.str();
            }
        }
    }

    return ISemanticsPart::analyseNext(program);
}

// GlobalSpaceCreation
std::string GlobalSpaceCreation::analyse(
        const std::vector<
            std::shared_ptr<mdl::Module>>& program) 
{


    std::map<std::string, std::shared_ptr<mdl::Module>> map;
    for (auto&& mod : program) {
        auto it = map.find(mod->name());
        if (it != map.end() && it->second->fileExtension() == "adb") {
            map[mod->name()] = mod;
        } else if (it == map.end()) {
            map[mod->name()] = mod;
        }
    }

    std::vector<std::shared_ptr<node::IDecl>> units;
    std::transform(
        map.begin(), map.end(),
        std::inserter(units, units.end()),
        [] (auto&& mod) { return mod.second->unit().lock(); });

    for (auto&& mod : program) {
        auto unit = mod->unit().lock();
        auto gp = 
            std::make_shared<node::GlobalSpace>(unit);
        mod->resetUnit(gp);
        auto [ok1, with] = addImportsPtrs_(mod, units);
        if (!ok1) {
            auto name = with->name().toString('.');
            std::stringstream ss;
            ss << mod->fileName();
            ss << ":";
            ss << "The module cannot be imported: ";
            ss << name;
            return ss.str();
        }
        auto [ok2, use] = addReduceImportPtrs_(mod);
        if (!ok2) {
            auto name = use->name().toString('.');
            std::stringstream ss;
            ss << mod->fileName();
            ss << ":";
            ss << "The name cannot be reduced: ";
            ss << name;
            return ss.str();
        }
    }

    return ISemanticsPart::analyseNext(program);
}

std::pair<bool, std::shared_ptr<node::With>> 
GlobalSpaceCreation::addImportsPtrs_(
    std::shared_ptr<mdl::Module> mod,
    const std::vector<std::shared_ptr<node::IDecl>>& units)
{
    std::map<std::string, std::shared_ptr<node::IDecl>> map;
    std::transform(
        units.begin(), units.end(), 
        std::inserter(map, map.end()), 
        [] (auto u) { return std::make_pair(u->name(), u); } );
    
    for (auto&& w : mod->with()) {
        auto name = w->name().toString('.');
        auto it = map.find(name);
        if (it == map.end()) {
            return {false, w};
        }
        auto unit = mod->unit().lock();
        if (auto space = 
            std::dynamic_pointer_cast<node::GlobalSpace>(unit)) 
        {
            space->addImport(it->second);
        }
        else 
        {
            throw std::logic_error(
                "Internal error in semantic_part.cpp");
        }
    }
    return {true, nullptr};
}

std::pair<bool,std::shared_ptr<node::Use>>
GlobalSpaceCreation::addReduceImportPtrs_(std::shared_ptr<mdl::Module> mod) {
    auto unit = mod->unit().lock();
    std::map<std::string, std::shared_ptr<node::IDecl>> imports;
    auto space = 
            std::dynamic_pointer_cast<node::GlobalSpace>(unit);
    if (space) {
        auto&& imp = space->imports();
        std::transform(imp.begin(), imp.end(), 
                       std::inserter(imports, imports.end()), 
                       [] (auto i) { return std::make_pair(i->name(), i); });
    } else {
        throw std::logic_error(
            "Internal error in semantic_part.cpp");
    }

    for (auto u : mod->use()) {
        auto name = u->name().toString('.');
        auto it = imports.find(name);
        if (it == imports.end()) {
            return {false, u};
        }
        if (auto pack 
                = std::dynamic_pointer_cast<node::PackDecl>(it->second)) 
        {
            for (auto d : *(pack->decls())) {
                space->addImport(d);
            }
        }
    }
    return {true, nullptr};
}

// CircularImportCheck
std::string CircularImportCheck::analyse(
    const std::vector<std::shared_ptr<mdl::Module>>& program)  
{
    auto main = dynamic_cast<node::GlobalSpace*>(
        program[0]->unit().lock().get());
    assert(main);
    std::vector<std::shared_ptr<node::IDecl>> onStack;
    if (!checkSpace_(main, onStack)) {
        return "There is a looped import in the program";
    }

    return ISemanticsPart::analyseNext(program);
}

bool CircularImportCheck::checkSpace_(
    node::GlobalSpace* cur, 
    std::vector<std::shared_ptr<node::IDecl>>& onStack)
{
    auto&& unit = cur->unit();
    auto it = std::find(onStack.begin(), onStack.end(), unit);
    if (it != onStack.end()) {
        return false;
    }
    onStack.push_back(unit);
    auto&& imports = cur->imports();
    for (auto&& import : imports) {
        if (auto* space 
            = dynamic_cast<node::GlobalSpace*>(import->parent())) 
        {
            if (!checkSpace_(space, onStack)) {
                return false;
            }
        }
    }
    onStack.pop_back();
    return true;
}

// NameConflictCheck 
std::string NameConflictCheck::analyse(
    const std::vector<
        std::shared_ptr<mdl::Module>>& program)
{
    for (auto&& mod : program) {
        auto space = 
                std::dynamic_pointer_cast<node::GlobalSpace>(mod->unit().lock());
        if (space) {
            auto res = analyseDecl_(space->unit());
            if (!res.empty()) {
                std::stringstream ss;
                ss << mod->fileName();
                ss << ":";
                ss << res;
                return ss.str();
            }
        } else {
            throw std::logic_error(
                "Internal error in semantic_part.cpp");
        }
    }

    return ISemanticsPart::analyseNext(program);
}
std::string NameConflictCheck::analyseDecl_( 
    std::shared_ptr<node::IDecl> decl)
{
    if (std::dynamic_pointer_cast<node::VarDecl>(decl) || 
        std::dynamic_pointer_cast<node::TypeAliasDecl>(decl))  
    { return ""; }

    std::map<std::string, std::shared_ptr<node::IDecl>> nameNDecl;
    std::vector<std::shared_ptr<node::IDecl>> allDecls;
    if (auto proc = 
            std::dynamic_pointer_cast<node::ProcBody>(decl)) 
    {   
        allDecls.insert(allDecls.end(), 
                        proc->params().begin(), 
                        proc->params().end());
        allDecls.insert(allDecls.end(), 
                        proc->decls()->begin(), 
                    proc->decls()->end());
    } else {
        if (auto record = 
                std::dynamic_pointer_cast<node::RecordDecl>(decl)) 
        {
            auto beg = record->decls()->begin();
            auto end = record->decls()->begin();
            allDecls.insert(allDecls.end(), beg, end);

        } 
        else if (auto pack = 
                    std::dynamic_pointer_cast<node::PackDecl>(decl))
        {
            auto beg = pack->decls()->begin();
            auto end = pack->decls()->begin();
            allDecls.insert(allDecls.end(), beg, end);

            if (auto priv = pack->privateDecls()) {
                allDecls.insert(allDecls.end(), priv->begin(), priv->end());
            }
        }
    }

    for (auto&& d : allDecls) {
        auto it = nameNDecl.find(d->name());
        if (it != nameNDecl.end() && 
                (
                    (!std::dynamic_pointer_cast<node::ProcBody>(d) || 
                     !std::dynamic_pointer_cast<node::ProcBody>(it->second)) &&
                // проверку на боди и декл пака при совпадении имен 
                    (
                        (std::dynamic_pointer_cast<node::PackDecl>(d) && 
                        !std::dynamic_pointer_cast<node::PackBody>(it->second) &&
                        std::dynamic_pointer_cast<node::PackDecl>(it->second) &&
                        !std::dynamic_pointer_cast<node::PackBody>(d)) || 

                        (std::dynamic_pointer_cast<node::PackBody>(d) && 
                        std::dynamic_pointer_cast<node::PackBody>(it->second))
                    )
                )
            )
        {
            std::stringstream ss;
            ss << "Name conflict: ";
            ss << "name: " << d->name() << ". ";
            ss << "In decl: " << decl->name();
            return ss.str();
        } else {
            nameNDecl[d->name()] = d;
        }
    }

    for (auto&& d : allDecls) {
        auto res = analyseDecl_(d);
        if (!res.empty()) {
            return res;
        }
    }

    return "";
} 

// PackBodyNDeclLinking
std::string PackBodyNDeclLinking::analyse(
    const std::vector<std::shared_ptr<mdl::Module>>& program)
{
    auto res = analyseProgram_(program);
    if (!res.empty()) {
        return res;
    }

    for (auto&& mod : program) {
        auto space = 
                std::dynamic_pointer_cast<node::GlobalSpace>(mod->unit().lock());
        if (space) {
            auto res = analyseContainer_(space->unit());
            if (!res.empty()) {
                std::stringstream ss;
                ss << mod->fileName();
                ss << ":";
                ss << res;
                return ss.str();
            }
        } else {
            throw std::logic_error(
                "Internal error in semantic_part.cpp");
        }
    }

    return ISemanticsPart::analyseNext(program);
}

static void analysePackDecl(
    std::shared_ptr<node::PackDecl> decl, 
    std::map<attribute::QualifiedName, std::shared_ptr<node::PackDecl>>& map,
    attribute::QualifiedName name)
{   
    name.push(decl->name());
    map[name] = decl;
    for (auto&& d : *decl->decls()) {
        if (auto pack = std::dynamic_pointer_cast<node::PackDecl>(d)) {
            analysePackDecl(pack, map, name);
        }
    }
    if (decl->privateDecls()) {
        for (auto&& d : *decl->privateDecls()) {
            if (auto pack = std::dynamic_pointer_cast<node::PackDecl>(d)) {
                analysePackDecl(pack, map, name);
            }
        }
    }

} 

static std::string analysePackBody(    
    std::shared_ptr<node::PackBody> body, 
    std::map<attribute::QualifiedName, std::shared_ptr<node::PackDecl>>& map,
    attribute::QualifiedName name)
{   
    name.push(body->name());
    auto it = map.find(name);
    if (it != map.end()) {
        auto decl = it->second;
        decl->setPackBody(body);
        body->setPackDecl(decl);
    } else {
        std::stringstream ss;
        ss << "It is impossible to link the package body: ";
        ss << name.toString('.');
        ss << " With its declaration";
        return ss.str();
    }

    for (auto&& d : *body->decls()) {
        if (auto body = std::dynamic_pointer_cast<node::PackBody>(d)) {
            auto res = analysePackBody(body, map, name);
            if (!res.empty()) {
                return res;
            }
        } else if (auto decl = std::dynamic_pointer_cast<node::PackBody>(d)) {
            analysePackDecl(decl, map, name);
        }
    }

    return "";
}

std::string PackBodyNDeclLinking::analyseContainer_(
    std::shared_ptr<node::IDecl> decl)
{      
    std::shared_ptr<node::DeclArea> decls;
    if (auto proc = std::dynamic_pointer_cast<node::ProcBody>(decl)) {
        decls = proc->decls();
    } else if (auto func = std::dynamic_pointer_cast<node::FuncBody>(decl)) {
        decls = func->decls();
    } else if (auto packBody = std::dynamic_pointer_cast<node::PackBody>(decl)) {
        decls = packBody->decls();
    } else {
        return "";
    }
    
    std::map<attribute::QualifiedName, std::shared_ptr<node::PackDecl>> map;
    std::shared_ptr<node::PackDecl> packDecl;
    for (auto&& d : *decls) {
        if ((packDecl = std::dynamic_pointer_cast<node::PackDecl>(d)) && 
            !std::dynamic_pointer_cast<node::PackBody>(d)) 
        {
            analysePackDecl(packDecl, map, decl->name());
        } 
        else if (auto packBody = std::dynamic_pointer_cast<node::PackBody>(d)) 
        {
            auto res = analysePackBody(packBody, map, decl->name());
            if (!res.empty()) {
                return res;
            }
        }
        if (!std::dynamic_pointer_cast<node::PackDecl>(d)) {
            auto res = analyseContainer_(d);
            if (!res.empty()) {
                return res;
            }
        }
    }

    return "";
}

std::string PackBodyNDeclLinking::analyseProgram_(
    const std::vector<std::shared_ptr<mdl::Module>>& program) 
{
    std::map<attribute::QualifiedName, std::shared_ptr<node::PackDecl>> map;
    for (auto&& mod : program) {
        auto space = std::dynamic_pointer_cast<node::GlobalSpace>(mod->unit().lock());
        assert(space && "No space in mod");
        auto unit = space->unit();
        if (auto packDecl = std::dynamic_pointer_cast<node::PackDecl>(unit)) {
            analysePackDecl(packDecl, map, space->name());
        }
    }

    for (auto&& mod : program) {
        auto space = std::dynamic_pointer_cast<node::GlobalSpace>(mod->unit().lock());
        assert(space && "No space in mod");
        auto unit = space->unit();
        if (auto packBody = std::dynamic_pointer_cast<node::PackBody>(unit)) {
            auto res = analysePackBody(packBody, map, space->name());
            if (!res.empty()) {
                return res;
            }
        }
    }
    return "";
}

// TypeNameToRealType 
std::string 
TypeNameToRealType::analyse(
        const std::vector<
            std::shared_ptr<mdl::Module>>& program)
{
    for (auto& mod : program) {
        auto unit = mod->unit().lock();
        auto space = 
                std::dynamic_pointer_cast<node::GlobalSpace>(unit);
        if (space) {
            auto unit = space->unit();
            auto res = analyseContainer_(unit);
            if (!res.empty()) {
                std::stringstream ss;
                ss << mod->fileName();
                ss << ":";
                ss << res;
                return ss.str();
            }
        } else {
            throw std::logic_error(
                "Internal error in semantic_part.cpp");
        }
    }
    return ISemanticsPart::analyseNext(program);
}

std::string 
TypeNameToRealType::analyseContainer_(
    std::shared_ptr<node::IDecl> decl)
{   
    // std::shared_ptr<node::DeclArea> decls;
    std::vector<std::shared_ptr<node::IDecl>> decls;
    if (auto proc = 
            std::dynamic_pointer_cast<node::ProcBody>(decl)) 
    {
        for (auto&& par : proc->params()) {
            auto res = analyseParam_(par);
            if (!res.empty()) {
                std::stringstream ss;
                ss << decl->name() << ':';
                ss << res;
                return ss.str();
            }
        }
        decls.insert(decls.end(), proc->decls()->begin(), proc->decls()->end());
    } 
    else if (auto record
                = std::dynamic_pointer_cast<node::RecordDecl>(decl))
    {
        decls.insert(decls.end(), proc->decls()->begin(), proc->decls()->end());
    }
    else if (auto pack =
                std::dynamic_pointer_cast<node::PackDecl>(decl))
    {
        decls.insert(decls.end(), pack->decls()->begin(), pack->decls()->end());
        if (auto priv = pack->privateDecls()) {
            decls.insert(decls.end(), priv->begin(), priv->end());
        }
    }

    for (auto&& d : decls) {
        if (std::dynamic_pointer_cast<node::TypeAliasDecl>(d) || 
            std::dynamic_pointer_cast<node::VarDecl>(d) ||
            std::dynamic_pointer_cast<node::RecordDecl>(d))
        {
            auto res = analyseDecl_(d);
            if (!res.empty()) {
                std::stringstream ss;
                ss << decl->name() << ':';
                ss << res;
                return ss.str();
            }
        } 
        else 
        {
            auto res = analyseContainer_(d);
            if (!res.empty()) {
                std::stringstream ss;
                ss << decl->name() << '.';
                ss << res;
                return ss.str();
            }
        }
    }

    if (auto func = std::dynamic_pointer_cast<node::FuncBody>(decl)) {
        auto res = analyseDecl_(decl);
        if (!res.empty()) {
            std::stringstream ss;
            ss << res;
            return ss.str();
        }
    }

    return "";
}

std::string 
TypeNameToRealType::analyseDecl_(
    std::shared_ptr<node::IDecl> decl)
{   
    std::shared_ptr<node::TypeAliasDecl> alias;
    std::shared_ptr<node::VarDecl> var;
    std::shared_ptr<node::TypeName> typeName;
    std::shared_ptr<node::FuncBody> func;
    if (alias = 
            std::dynamic_pointer_cast<node::TypeAliasDecl>(decl)) 
    {
        auto origin = alias->origin();
        if (typeName = 
                std::dynamic_pointer_cast<node::TypeName>(origin));
        else if (auto arrayType = 
                    std::dynamic_pointer_cast<node::ArrayType>(origin))
        {
            return analyseArrayType_(arrayType, 
                dynamic_cast<node::IDecl*>(decl->parent()),
                decl);
        }
    } 
    else if (var = 
                std::dynamic_pointer_cast<node::VarDecl>(decl)) 
    {
        if (typeName = 
                std::dynamic_pointer_cast<node::TypeName>(var->type()));
        else if (auto arrayType = 
                    std::dynamic_pointer_cast<node::ArrayType>(var->type()))
        {
            return analyseArrayType_(arrayType, 
                dynamic_cast<node::IDecl*>(decl->parent()),
                decl);
        }
    }
    else if (auto record = 
                std::dynamic_pointer_cast<node::RecordDecl>(decl)) 
    {
        return analyseRecord_(record);
    } 
    else if (func = std::dynamic_pointer_cast<node::FuncBody>(decl)) 
    {
        if (typeName = 
                std::dynamic_pointer_cast<node::TypeName>(func->retType()));
        else if (auto arrayType = 
                    std::dynamic_pointer_cast<node::ArrayType>(func->retType()))
        {
            return "it is forbidden to use an" 
                   " explicitly specified array" 
                   " type as a return value for a function";
        }
    } 
    else 
    {
        return "";
    }

    if (typeName) {
        if (!typeName->hasName()) {
            std::stringstream ss;
            ss << "It is forbidden to" 
                  " use superclass reference" 
                  " for aliases, vars, arrays and return type: ";
            ss << alias->name();
            ss << " : ";
            ss << typeName->attribute().toString();
            return ss.str();
        }
        auto parent = dynamic_cast<node::IDecl*>(decl->parent());
        auto declsInSpaces = 
            parent->reachable(typeName->name(), decl);
        bool isTypeSet = false;
        if (!declsInSpaces.empty()) {
            auto&& decls = declsInSpaces.front();
            auto record = 
                std::dynamic_pointer_cast<node::RecordDecl>(decls[0]);
            auto als = 
                std::dynamic_pointer_cast<node::TypeAliasDecl>(decls[0]);
            if (alias) {
                if (als == alias) {
                    std::stringstream ss;
                    ss << "It is forbidden to use the" 
                          " same alias as the" 
                          " type for aliases, types";
                    ss << alias->name();
                    ss << " : ";
                    ss << typeName->attribute().toString();
                    return ss.str();
                }
                if (record) alias->resetOrigin(record);
                else if (als) alias->resetOrigin(als);
            } else if (var) {
                if (record) var->resetType(record);
                else if (als) var->resetType(als);
            } else if (func) {
                // разрешение возвр. значение для ф-ции
                if (record) func->resetRetType(record);
                else if (als) func->resetRetType(als);
            }
            if (record || als) {
                isTypeSet = true;
            }
        }
        if (!isTypeSet) {
            std::stringstream ss;
            ss << "An unresolved type name:";
            ss << typeName->name().toString('.');
            return ss.str();
        }
    } 

    return "";
}

std::string 
TypeNameToRealType::analyseArrayType_(
    std::shared_ptr<node::ArrayType> atype, 
    node::IDecl* space,
    std::shared_ptr<node::IDecl> parent)
{
    auto type = atype->type();
    if (auto typeName = 
            std::dynamic_pointer_cast<node::TypeName>(type)) 
    {
        if (!typeName->hasName()) {
            std::stringstream ss;
            ss << "It is forbidden to" 
                    " use superclass reference" 
                    " for aliases, vars, arrays: ";
            ss << " : ";
            ss << typeName->attribute().toString();
            return ss.str();
        }
        bool isTypeSet = false;
        auto declsInSpaces = 
            space->reachable(typeName->name(), parent);
        if (!declsInSpaces.empty()) {
            auto&& decls = declsInSpaces.front();
            auto record = 
                std::dynamic_pointer_cast<node::RecordDecl>(decls[0]);
            auto als = 
                std::dynamic_pointer_cast<node::TypeAliasDecl>(decls[0]);

            if (record) atype->resetType(record);
            else if (als) atype->resetType(als);

            if (record || als) {
                isTypeSet = true;
            }
        }
        if (!isTypeSet) {
            std::stringstream ss;
            ss << "An unresolved type name:";
            ss << typeName->name().toString('.');
            return ss.str();
        }
    } 
    else if (auto arrayType = 
                std::dynamic_pointer_cast<node::TypeName>(type))
    {
        return "it is forbidden to" 
               " use an array for an array type";
    }
    return "";
}

std::string 
TypeNameToRealType::analyseRecord_(
    std::shared_ptr<node::RecordDecl> decl,
    std::shared_ptr<node::RecordDecl> derived)
{
    if (!decl->isInherits()) return "";
    if (decl->isInherits() && !decl->base().expired()) return "";

    auto&& baseName = decl->baseName();
    auto&& space = dynamic_cast<node::IDecl*>(decl->parent());
    auto&& selfDecl = std::dynamic_pointer_cast<node::IDecl>(decl->self());
    auto&& declsInSpaces = space->reachable(baseName, selfDecl);
    bool isBaseSet = false;

    if (!declsInSpaces.empty()) {
        auto&& decls = declsInSpaces.front();
        if (auto base = 
                std::dynamic_pointer_cast<node::RecordDecl>(decls[0]))
        {
            if (base->isInherits()) {
                auto res = analyseRecord_(base, decl);
                if (!res.empty()) {
                    return res;
                }
            }
            if (!base->isTagged()) {
                std::stringstream ss;
                ss << "Inheritance is not possible" 
                      " from an untagged record: ";
                ss << "Record: " << decl->name();
                ss << ". Base Name: " << baseName.toString('.');
                return ss.str();
            }

            if (base == derived) {
                std::stringstream ss;
                ss << "Mutual inheritance is prohibited";
                ss << "Record 1: " << decl->name();
                ss << ". Record 2: " << derived->name();
                return ss.str();
            } 

            isBaseSet = true;
            decl->setBase(base);
        }
    }

    if (!isBaseSet) {
        std::stringstream ss;
        ss << "Unresolved name during inheritance: ";
        ss << "Record: " << decl->name();
        ss << ". Base Name: " << baseName.toString('.');
        return ss.str();
    } 

    decl->setTagged();

    return "";
}

std::string 
TypeNameToRealType::analyseParam_(
    std::shared_ptr<node::VarDecl> decl) 
{   
    auto type = decl->type();
    if (auto typeName = 
            std::dynamic_pointer_cast<node::TypeName>(type)) 
    {
        if (typeName->hasName()) {
            return analyseDecl_(decl);
        } 

        auto&& attr = typeName->attribute();
        if (attr.right() != "class") {
            return "An attribute as a type is" 
                   " allowed only as a superclass" 
                   " reference: [NAME]'class";
        }
        auto classRef = 
            std::make_shared<node::SuperclassReference>(attr);
        
        decl->resetType(classRef);
    } else if (std::dynamic_pointer_cast<node::ArrayType>(type)) {
        return "Anonymous array definition not allowed";
    }

    return "";
}

// InheritsVarNameConlflicCheck
std::string
InheritsVarNameConlflicCheck::analyse(
    const std::vector<
    std::shared_ptr<mdl::Module>>& program)
{
    for (auto mod : program) {
        auto unit = mod->unit().lock();
        auto space = 
                std::dynamic_pointer_cast<node::GlobalSpace>(unit);
        auto res = analyseContainer_(space->unit());
        if (!res.empty()) {
            std::stringstream ss;
            ss << mod->fileName();
            ss << ":";
            ss << res;
            return ss.str();
        }
    }
    return ISemanticsPart::analyseNext(program);
}

std::string 
InheritsVarNameConlflicCheck::analyseContainer_(
    std::shared_ptr<node::IDecl> decl)
{
    std::vector<std::shared_ptr<node::IDecl>> decls;
    if (auto proc = 
            std::dynamic_pointer_cast<node::ProcBody>(decl)) 
    {
        decls.insert(decls.end(), proc->decls()->begin(), proc->decls()->end());
    } 
    else if (auto pack =
                std::dynamic_pointer_cast<node::PackDecl>(decl))
    {
        decls.insert(decls.end(), pack->decls()->begin(), pack->decls()->end());
        if (auto priv = pack->privateDecls()) {
            decls.insert(decls.end(), priv->begin(), priv->end());
        }
    } else {
        return "";
    }

    for (auto&& d : decls) {
        if (auto rec = std::dynamic_pointer_cast<node::RecordDecl>(d)) {
            std::set<std::string> allVars;
            auto curRec = rec;
            while (curRec) {
                for (auto&& v : *(curRec->decls())) {
                    if (allVars.contains(v->name())) {
                        std::stringstream ss;
                        ss << "Redefining a name in a derived type";
                        ss << ":";
                        ss << " Base: ";
                        ss << curRec->name();
                        ss << ". Var: ";
                        ss << v->name();
                        return ss.str();
                    }
                    allVars.insert(v->name());
                }
                curRec = curRec->base().lock();
            }
        }

        auto res = analyseContainer_(d);
        if (!res.empty()) {
            return res;
        }
    }

    return "";
}

// OverloadCheck
std::string 
OverloadCheck::analyse(
    const std::vector<
        std::shared_ptr<mdl::Module>>& program) 
{
    for (auto mod : program) {
        auto unit = mod->unit().lock();
        auto space = 
                std::dynamic_pointer_cast<node::GlobalSpace>(unit);
        auto res = analyseContainer_(space->unit());
        if (!res.empty()) {
            std::stringstream ss;
            ss << mod->fileName();
            ss << ":";
            ss << res;
            return ss.str();
        }
    }
    return ISemanticsPart::analyseNext(program);
}

std::string 
OverloadCheck::analyseContainer_(
    std::shared_ptr<node::IDecl> decl)
{
    std::vector<std::shared_ptr<node::IDecl>> decls;
    if (auto proc = 
            std::dynamic_pointer_cast<node::ProcBody>(decl)) 
    {
        decls.insert(decls.end(), proc->decls()->begin(), proc->decls()->end());
    } 
    else if (auto pack =
                std::dynamic_pointer_cast<node::PackDecl>(decl))
    {
        decls.insert(decls.end(), pack->decls()->begin(), pack->decls()->end());
        if (auto priv = pack->privateDecls()) {
            decls.insert(decls.end(), priv->begin(), priv->end());
        }
    } else {
        return "";
    }

    std::map<std::string, std::vector<std::shared_ptr<node::IDecl>>> nameNDecls;
    for (auto&& d : decls) {
        nameNDecls[d->name()].push_back(d);
    }

    for (auto&& [_, decls] : nameNDecls) {
        if (decls.size() > 1 && !std::dynamic_pointer_cast<node::PackDecl>(decls.front())) {
            for (std::size_t i = 0; i < decls.size() - 1; ++i) {
                auto&& d1 = decls[i];
                auto proc1 = 
                        std::dynamic_pointer_cast<node::ProcBody>(d1);
                auto&& params1 = proc1->params();
                auto isFunc1 = std::dynamic_pointer_cast<node::FuncBody>(proc1);
                for (std::size_t j = i + 1; j < decls.size(); ++j) {
                    auto&& d2 = decls[j];
                    auto proc2 = 
                        std::dynamic_pointer_cast<node::ProcBody>(d2);
                    auto isFunc2 = std::dynamic_pointer_cast<node::FuncBody>(proc2);
                    
                    if ((isFunc1 && !isFunc2) || (!isFunc1 && isFunc2)) {
                        continue;
                    }
                    
                    auto&& params2 = proc2->params();
                    if (params1.size() != params2.size()) {
                        continue;
                    } 

                    bool eq = false;
                    for (std::size_t k = 0; k < params1.size(); ++k) {
                        eq = params1[k]->type()->compare(params2[k]->type());
                        if (!eq) {
                            break;
                        }
                    }
                    if (eq) {
                        std::stringstream ss;
                        ss << decl->name();
                        ss << " Not an overload, but a name conflict: ";
                        ss << proc1->name();
                        return ss.str();
                    }
                } 
            }
        }
    }

    for (auto&& d : decls) {
        if (std::dynamic_pointer_cast<node::PackDecl>(d) || 
            std::dynamic_pointer_cast<node::ProcBody>(d))
        {
            auto res = analyseContainer_(d);
            if (!res.empty()) {
                std::stringstream ss;
                ss << decl->name();
                ss << '.';
                ss << res;
                return ss.str();
            }
        }
    }

    return "";
}

// SubprogrBodyNDeclLinking
std::string SubprogBodyNDeclLinking::analyse(
    const std::vector<std::shared_ptr<mdl::Module>>& program) 
{
    for (auto mod : program) {
        auto unit = mod->unit().lock();
        auto space = 
                std::dynamic_pointer_cast<node::GlobalSpace>(unit);
        auto res = analyseContainer_(space->unit());
        if (!res.empty()) {
            std::stringstream ss;
            ss << mod->fileName();
            ss << ":";
            ss << res;
            return ss.str();
        }
    }
    return ISemanticsPart::analyseNext(program);
}

std::string SubprogBodyNDeclLinking::analyseContainer_(
    std::shared_ptr<node::IDecl> decl)
{
    bool isPackDecl = false;
    std::vector<std::shared_ptr<node::IDecl>> decls;
    std::shared_ptr<node::PackDecl> pack;
    if (auto proc = std::dynamic_pointer_cast<node::ProcBody>(decl)) {
        decls.insert(decls.end(), proc->decls()->begin(), proc->decls()->end());
    } else if (pack = std::dynamic_pointer_cast<node::PackDecl>(decl)) {
        decls.insert(decls.end(), pack->decls()->begin(), pack->decls()->end());
        if (auto priv = pack->privateDecls()) {
            decls.insert(decls.end(), priv->begin(), priv->end());
        }
        isPackDecl = !std::dynamic_pointer_cast<node::PackBody>(decl);
    } else {
        return "";
    }

    std::map<std::string, std::vector<std::shared_ptr<node::IDecl>>> map;
    if (isPackDecl) {
        if (auto body = pack->packBody().lock()) {
            for (auto&& d : *body->decls()) {
                map[d->name()].push_back(d);
            }
        }        
    }
    for (auto&& d : decls) {
        if (isPackDecl) {
            auto it = map.find(d->name());

            auto packDeclProc = std::dynamic_pointer_cast<node::ProcDecl>(d);
            auto packDeclIsFunc = std::dynamic_pointer_cast<node::FuncDecl>(d);

            if (packDeclProc) {
                bool isLinking = false;

                if (it != map.end()) {
                    for (auto&& bodyD : it->second) {
                        if (auto packBodyProc = std::dynamic_pointer_cast<node::ProcBody>(bodyD)) {
                            auto packBodyIsFunc = std::dynamic_pointer_cast<node::FuncBody>(bodyD);

                            if (!((packBodyIsFunc && 1) ^ (packDeclIsFunc && 1))) {
                                auto&& params1 = packDeclProc->params();
                                auto&& params2 = packBodyProc->params();

                                if (params1.size() == params2.size()) {
                                    bool flag = true;
                                    for (std::size_t i = 0; i < params1.size(); ++i) {
                                        flag = params1[i]->type()->compare(params2[i]->type());
                                        if (!flag) {
                                            break;
                                        }
                                    }

                                    if (flag && packBodyIsFunc && packDeclIsFunc) {
                                        auto type1 = packDeclIsFunc->retType();
                                        auto type2 = packBodyIsFunc->retType();
                                        flag = type1->compare(type2);
                                    }

                                    if (flag) {
                                        if (packBodyIsFunc) {
                                            packDeclIsFunc->setBody(packBodyIsFunc);
                                        } else {
                                            packDeclProc->setBody(packBodyProc);
                                        }
                                        isLinking = true;
                                    }
                                }

                            }
                        }
                    }
                }

                if (!isLinking) {
                    std::stringstream ss;
                    ss << "For subprogram: ";
                    ss << packDeclProc->name();
                    ss << ". There are no definitions in the package body";
                    return ss.str();
                }
            } else if (it != map.end()) {
                std::stringstream ss;
                ss << "Redefining the declaration in the package body. Package: ";
                ss << pack->name();
                return ss.str();
            }
        }

        auto res = analyseContainer_(d);
        if (!res.empty()) {
            return res;
        }
    }



    return "";
}

// CreateClassDeclaration
std::string CreateClassDeclaration::analyse(
    const std::vector<std::shared_ptr<mdl::Module>>& program) 
{
    for (auto mod : program) {
        auto unit = mod->unit().lock();
        auto space = 
                std::dynamic_pointer_cast<node::GlobalSpace>(unit);
        auto res = analyseContainer_(space->unit().get());
        if (!res.empty()) {
            std::stringstream ss;
            ss << mod->fileName();
            ss << ":";
            ss << res;
            return ss.str();
        }
    }
    return ISemanticsPart::analyseNext(program);
}

std::string
CreateClassDeclaration::analyseContainer_(node::IDecl* decl) {
    std::vector<std::shared_ptr<node::IDecl>> decls;
    std::shared_ptr<node::DeclArea> declArea;
    if (auto pack = dynamic_cast<node::PackDecl*>(decl)) {
        decls.insert(decls.end(), pack->decls()->begin(), pack->decls()->end());
        if (auto priv = pack->privateDecls()) {
            decls.insert(decls.end(), priv->begin(), priv->end());
        }
        declArea = pack->decls();
    } else {
        if (auto proc = dynamic_cast<node::ProcBody*>(decl)) {
            for (auto&& d : *(proc->decls())) {
                auto res = analyseContainer_(d.get());
                if (!res.empty()) {
                    return res;
                }
            }
        }
        return "";
    }

    std::vector<std::shared_ptr<node::ClassDecl>> classes;

    for (auto&& d : decls) {  
        if (auto rec = std::dynamic_pointer_cast<node::RecordDecl>(d)) {
            if (rec->isTagged() && rec->cls().expired()) {
                if (rec->isInherits()) {
                    assert(!rec->base().expired());
                    auto base = rec->base().lock();
                    if (base->parent() != d->parent()) {
                        auto res = analyseContainer_(
                            dynamic_cast<node::IDecl*>(base->parent()));
                        if (!res.empty()) {
                            return res;
                        }
                        auto baseClass = base->cls().lock();
                        rec->cls().lock()->setBase(baseClass);
                    }
                }
                classes.push_back(std::make_shared<node::ClassDecl>(rec));
                rec->setClass(classes.back());
            }
        } else if (auto proc = std::dynamic_pointer_cast<node::ProcBody>(d)) {
            bool isProc = !std::dynamic_pointer_cast<node::FuncBody>(proc);    
            auto name = proc->name();
            std::vector<std::shared_ptr<node::IType>> params;
            std::shared_ptr<node::ClassDecl> cls;
            for (auto var : proc->params()) {
                auto vType = var->type();
                params.push_back(vType);
                auto rec = std::dynamic_pointer_cast<node::RecordDecl>(vType);
                if (!cls && rec) {
                    cls = rec->cls().lock();
                }
            }
            if (cls && !cls->containsMethod(name, params, isProc)) {
                cls->addMethod(proc);
            } 
        }
    }

    for (auto&& c : classes) {
        declArea->addDecl(c);
    }

    return "";
}

// OneClassInSubprogramCheckp
std::string 
OneClassInSubprogramCheck::analyse(
    const std::vector<
        std::shared_ptr<mdl::Module>>& program) 
{
    for (auto mod : program) {
        auto unit = mod->unit().lock();
        auto space = 
                std::dynamic_pointer_cast<node::GlobalSpace>(unit);
        auto res = analyseContainer_(space->unit());
        if (!res.empty()) {
            std::stringstream ss;
            ss << mod->fileName();
            ss << ":";
            ss << res;
            return ss.str();
        }
    }
    return ISemanticsPart::analyseNext(program);
}

std::string 
OneClassInSubprogramCheck::analyseContainer_(std::shared_ptr<node::IDecl> decl) {
    std::vector<std::shared_ptr<node::IDecl>> decls;
    if (auto proc = 
            std::dynamic_pointer_cast<node::ProcBody>(decl)) 
    {
        decls.insert(decls.end(), proc->decls()->begin(), proc->decls()->end());
    } 
    else if (auto pack =
                std::dynamic_pointer_cast<node::PackDecl>(decl))
    {
        decls.insert(decls.end(), pack->decls()->begin(), pack->decls()->end());
        if (auto priv = pack->privateDecls()) {
            decls.insert(decls.end(), priv->begin(), priv->end());
        }
    } else {
        return "";
    }

    for (auto&& d : decls) {  
        if (auto proc = std::dynamic_pointer_cast<node::ProcBody>(d)) {
            bool containsClass = false;
            bool isMethod = false;
            for (auto p : proc->params()) {
                auto type = p->type();
                if (auto rec = std::dynamic_pointer_cast<node::RecordDecl>(type)) {
                    if (auto cls = rec->cls().lock()) {
                        isMethod = isMethod || cls->parent() == d->parent();
                        if (containsClass && isMethod) {
                            std::stringstream ss;
                            ss << "A method cannot contain" 
                                  " more than 1 class in its arguments.";
                            ss << " Method: ";
                            ss << proc->name();
                            return ss.str();
                        }
                        containsClass = true;
                    }
                } else if (auto ref = std::dynamic_pointer_cast<node::SuperclassReference>(type)) {
                    // добавление в надклассовую ссылку указателя на класс
                    auto&& attr = ref->ref();
                    auto&& name = attr.left(); 
                    auto declsNSpaces = proc->reachable(name);
                    bool isClassSet = false;
                    if (!declsNSpaces.empty()) {
                        auto&& decls = declsNSpaces.front();
                        if (auto rec = std::dynamic_pointer_cast<node::RecordDecl>(decls[0])) {
                            auto cls = rec->cls().lock();
                            if (cls) {
                                ref->setClass(cls);
                                isClassSet = true;
                            }
                        }
                    }
                    if (!isClassSet) {
                        std::stringstream ss;
                        ss << "Unresolved class name" 
                              " in a superclass reference.";
                        ss << " Class name: ";
                        ss << name.toString('.');
                        ss << " Subprogram: ";
                        ss << proc->name();
                        return ss.str();
                    }
                }
            }
        }

        auto res = analyseContainer_(d);
        if (!res.empty()) {
            return res;
        }
    }

    return "";
}


// LinkExprs 
std::string LinkExprs::analyse(
        const std::vector<
            std::shared_ptr<mdl::Module>>& program)     
{

}


//  для assign:
//      для правой части vardecl и assign нужно rval по итогу
//      для левой части нужно lval по итогу

// для mbcall
//  нужно noval по итогу

// для return
//  нужно rval по итогу 

// for 
// создать переменную типа Integer

// while 
// нужно lval по итогу


// * у всех expr родитель подпрог, либо пакет, либо рерорд
// Получаем expr
// 

// op из dot -  последовательность из имен или имен с параметрами
// * просто имя - переменная, часть квал. имени, вызов
// * имя с параметрами - вызов или индексация

// если mbcall
std::string analyseContainer_(std::shared_ptr<node::IDecl> decl) {
    std::shared_ptr<node::Body> body;
}

std::string LinkExprs::analyseVarDecl_(std::shared_ptr<node::VarDecl> var) {
    auto rhs = var->rval();
    if (!rhs) {
        return "";
    }

    if (rhs) {
        
    }
}

//m 4 + 2 + 3 + 2 + 3 + 2 + 2 + 2
//r 3 + 3 + 3 + 4 + 2 + 1 + 1 + 2 + 4

// NameExpr
// AttributeExpr
// CallOrIdxExpr
// Op
std::pair<std::string, std::shared_ptr<node::IExpr>> 
LinkExprs::analyseExpr_(std::shared_ptr<node::IExpr> expr) {
    // auto op = 

    // if (auto name = std::dynamic_pointer_cast<node::NameExpr>())
}

std::pair<std::string, std::shared_ptr<node::IExpr>> 
LinkExprs::analyseOp_(std::shared_ptr<node::Op> op) { // dotop либо op
    auto oper = op->op();
    auto left = op->left();
    auto right = op->right();

    
}

std::string 
LinkExprs::analyseOpExprErr_(std::shared_ptr<node::IExpr> expr) {
    std::shared_ptr<node::Op> op;
    if (!(op = std::dynamic_pointer_cast<node::Op>(expr))) {
        return "";
    }

    if (op->op() == node::OpType::DOT &&
        !std::dynamic_pointer_cast<node::NameExpr>(expr) && 
        !std::dynamic_pointer_cast<node::CallOrIdxExpr>(expr)) 
    {
        return "Invalid operands for Dot Op";
    }

    if (op->op() == node::OpType::DOT &&
        (op->left() && op->left()->inBrackets()) || 
        (op->right() && op->right()->inBrackets())) 
    {
        return "One of the operands of Dot Op in parentheses";
    }

    auto res = analyseOpExprErr_(op->left());
    if (!res.empty()) return res;
    
    return analyseOpExprErr_(op->right());
}

} // semantics_part
