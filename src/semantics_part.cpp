#include "semantics_part.hpp"

#include <sstream>
#include <ranges>
#include <set>
#include <iterator>
#include <algorithm>
#include <tuple>

#include "node.hpp"
#include "string_utility.hpp"

namespace semantics_part {

// EntryPointCheck
std::string EntryPointCheck::analyse(
        const std::vector<
                std::shared_ptr<mdl::Module>>& program) 
{   
    auto unit = program[1]->unit().lock();
    if (!std::dynamic_pointer_cast<node::ProcBody>(unit) ||
         std::dynamic_pointer_cast<node::FuncBody>(unit)) 
    {
        return program[1]->fileName()
                + ": The entry point should be a procedure";
    }
    return ISemanticsPart::analyseNext(program);
}

// ModuleNameCheck
std::string ModuleNameCheck::analyse(
        const std::vector<
                std::shared_ptr<mdl::Module>>& program) 
{   
    for (auto&& mod : program | std::views::drop(2)) {
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
    for (auto&& mod : program | std::views::drop(1)) {
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
    for (auto&& mod : program | std::views::drop(1)) {
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

    for (auto&& mod : program | std::views::drop(1)) {
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

    std::vector<std::shared_ptr<mdl::Module>> units;
    std::transform(
        map.begin(), map.end(),
        std::inserter(units, units.end()),
        [] (auto&& mod) { return mod.second; });

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
    const std::vector<std::shared_ptr<mdl::Module>>& units)
{
    std::map<std::string, std::shared_ptr<mdl::Module>> map;
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
            auto unit = it->second->unit().lock();
            if (auto space = std::dynamic_pointer_cast<node::GlobalSpace>(unit)) {
                unit = space->unit();
            }
            space->addImport(unit);
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
        auto name = u->name().first() == "ada" ? "ada" : u->name().toString('.');
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
            if (u->name().toString('.') == "ada.text_io") {
                auto textio = std::dynamic_pointer_cast<node::PackDecl>(*(pack->decls()->begin()));
                for (auto d : *(textio->decls())) {
                    space->addImport(d);
                }
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
        program[1]->unit().lock().get());
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
    for (auto&& mod : program | std::views::drop(1)) {
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
                    ((!std::dynamic_pointer_cast<node::ProcBody>(d) || 
                      !std::dynamic_pointer_cast<node::ProcBody>(it->second)) && 
                      (!std::dynamic_pointer_cast<node::PackDecl>(d) && 
                       !std::dynamic_pointer_cast<node::PackDecl>(it->second))) ||
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

    for (auto&& mod : program | std::views::drop(1)) {
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
        auto packDecl = std::dynamic_pointer_cast<node::PackDecl>(unit);
        auto packBody = std::dynamic_pointer_cast<node::PackBody>(unit);
        if (packDecl && !packBody) {
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
    for (auto&& mod : program | std::views::drop(1)) {
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
        decls.insert(decls.end(), record->decls()->begin(), record->decls()->end());
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
        auto res = analyseContainer_(record);
        if (!res.empty()) {
            return res;
        }
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
            parent->reachable(typeName->name(), decl.get());
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
                if (record.get() == var->parent()) {
                    return "A record-type variable is" 
                           " nested within its own record-type: " 
                            + typeName->name().toString('.');
                }

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
            space->reachable(typeName->name(), parent.get());
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
    auto&& declsInSpaces = space->reachable(baseName, selfDecl.get());
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
    for (auto&& mod : program | std::views::drop(1)) {
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
    for (auto&& mod : program | std::views::drop(1)) {
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
                        auto type1 = params1[k]->type();
                        auto type2 = params2[k]->type();

                        if (!(eq = type1->compare(type2))) {
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
    for (auto&& mod : program | std::views::drop(1)) {
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
                                        flag = params1[i]->type()->compare(params2[i]->type()) && 
                                                params1[i]->in() == params2[i]->in() && 
                                                params1[i]->out() == params2[i]->out();
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
    for (auto&& mod : program | std::views::drop(1)) {
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
                std::shared_ptr<node::ClassDecl> baseClass;
                if (rec->isInherits()) {
                    assert(!rec->base().expired());
                    auto base = rec->base().lock();
                    if (base->parent() != d->parent()) {
                        auto res = analyseContainer_(
                            dynamic_cast<node::IDecl*>(base->parent()));
                        if (!res.empty()) {
                            return res;
                        }
                    }
                    baseClass = base->cls().lock();
                }
                classes.push_back(std::make_shared<node::ClassDecl>(rec));
                rec->setClass(classes.back());
                rec->cls().lock()->setBase(baseClass);
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
                proc->setClass(cls);
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
    for (auto&& mod : program | std::views::drop(1)) {
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
            int idx = 0;
            for (auto p : proc->params()) {
                auto type = p->type();
                if (auto rec = std::dynamic_pointer_cast<node::RecordDecl>(type)) {
                    if (auto cls = rec->cls().lock()) {
                        // один аргумент с типом тагед рекорда и на первом месте
                        if (containsClass || idx++ > 0) {
                            std::stringstream ss;
                            ss << "A method cannot contain" 
                                  " more than 1 class in its arguments.";
                            ss << " Method: ";
                            ss << proc->name();
                            ss << ". In this implementation,"
                                  " the class argument must" 
                                  " be placed first in the argument list.";
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
    for (auto&& mod : program | std::views::drop(1)) {
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

// какие проверки и линки есть (для вызова из блоков анализа):
/*
 * 1. Линковка аргументов вызовов (с анализом на ошибки)
 * 2. Анализ неправильных операций
 * 3. Линковка операций
 * 4. Линковка выражений
*/
std::string LinkExprs::analyseContainer_(std::shared_ptr<node::IDecl> decl) {
    std::vector<std::shared_ptr<node::VarDecl>> args;
    std::shared_ptr<node::Body> body;
    std::shared_ptr<node::DeclArea> decls;

    if (auto proc = std::dynamic_pointer_cast<node::ProcBody>(decl)) {
        args = proc->params();
        body = proc->body();
        decls = proc->decls();
    } else if (auto pack = std::dynamic_pointer_cast<node::PackDecl>(decl)) {
        decls = pack->decls();
    } else if (auto record = std::dynamic_pointer_cast<node::RecordDecl>(decl)) {
        decls = record->decls();
    } else {
        return "";
    }

    if (body) {
        auto res = analyseBody_(body, decls, args);
        if (!res.empty()) {
            return res;
        }
    }

    for (auto&& d : *decls) {
        if (auto var = std::dynamic_pointer_cast<node::VarDecl>(d)) {
            auto rhs = var->rval();
            if (rhs) {
                // анализ dot op
                auto op = std::dynamic_pointer_cast<node::Op>(rhs);
                if (op) {
                    auto opErr = analyseOpExprErr_(op);
                    if (!opErr.empty()) {
                        return opErr;
                    }
                }

                // линковка аргументов вызовов
                auto argsErr = analyseArgsExpr_(rhs);
                if (!argsErr.empty()) {
                    return argsErr;
                }

                // линковка
                attribute::QualifiedName _;
                auto [exprErr, newRhs] = op ? analyseOp_(op) : analyseExpr_(rhs, _);
                if (!exprErr.empty()) {
                    return exprErr;
                }

                // анализ in/out и rhs/lhs/novalue
                auto inOutValErr = 
                    analyseInOutRvalLvalNoVal_(args, newRhs, false, false);
                if (!inOutValErr.empty()) {
                    return inOutValErr;
                }
                
                var->setRval(newRhs);
            }
        } else {
            auto res = analyseContainer_(d);
            if (!res.empty()) {
                return res;
            }
        }
    }

    return "";
}

std::string LinkExprs::analyseBody_(
    std::shared_ptr<node::Body> body, 
    std::shared_ptr<node::DeclArea> parDecls,
    const std::vector<std::shared_ptr<node::VarDecl>>& args)
{
    if (!body) {
        return "";
    }

    std::string err;
    auto getRes = [this, &args, &err] 
    (std::shared_ptr<node::IExpr> expr, bool lhs, bool noVal) mutable 
        -> std::shared_ptr<node::IExpr> 
    {
        // проверка dot op
        auto op = std::dynamic_pointer_cast<node::Op>(expr);
        if (op) {
            err = analyseOpExprErr_(op);
            if (!err.empty()) {
                return nullptr;
            }
        } else if (auto call = std::dynamic_pointer_cast<node::CallOrIdxExpr>(expr)) {
            if (!std::dynamic_pointer_cast<node::NameExpr>(call->name()))  {
                err = "Using a subprogram call without" 
                      " a qualifying name is" 
                      " not supported in this implementation";
                return nullptr;
            }
        }

        // линковка аргументов вызовов
        err = analyseArgsExpr_(expr);
        if (!err.empty()) {
            return nullptr;
        }

        // линковка
        attribute::QualifiedName _;
        std::shared_ptr<node::IExpr> newExpr;
        std::tie(err, newExpr) = op ? analyseOp_(op) : analyseExpr_(expr, _);
        if (!err.empty()) {
            return nullptr;
        }

        // анализ in/out и rhs/lhs/novalue
        err = analyseInOutRvalLvalNoVal_(args, newExpr, lhs, noVal);
        if (!err.empty()) {
            return nullptr;
        }

        return newExpr;
    };
    // анализ статементов
    for (auto&& stm : *body) {  
        if (auto if_ = std::dynamic_pointer_cast<node::If>(stm)) {
            auto newCond = getRes(if_->cond(), false, false);
            if (!err.empty()) {
                return err;
            }
            if_->setCond(newCond);

            auto res = analyseBody_(if_->body(), parDecls, args);
            if (!res.empty()) {
                return res;
            }

            auto elseRes = analyseBody_(if_->bodyElse(), parDecls, args);
            if (!elseRes.empty()) {
                return elseRes;
            }

            for (auto&& [cond, body] : if_->elsifs()) {
                auto newCond = getRes(cond, false, false);
                if (!err.empty()) {
                    return err;
                }
                cond = newCond;

                auto res = analyseBody_(body, parDecls, args);
                if (!res.empty()) { 
                    return res;
                }
            }
        } else if (auto while_ = std::dynamic_pointer_cast<node::While>(stm)) {
            auto newCond = getRes(while_->cond(), false, false);
            if (!err.empty()) {
                return err;
            }

            while_->setCond(newCond);

            auto res = analyseBody_(while_->body(), parDecls, args);
            if (!res.empty()) {
                return res;
            }
        } else if (auto for_ = std::dynamic_pointer_cast<node::For>(stm)) {
            auto intTy = 
                std::make_shared<node::SimpleLiteralType>(node::SimpleType::INTEGER);
            auto var = std::make_shared<node::VarDecl>(for_->init(), intTy);
            parDecls->addDeclToFront(var);

            auto [expr1, expr2] = for_->range();

            auto newExrp1 = getRes(expr1, false, false);
            if (!err.empty()) {
                return err;
            }

            auto newExrp2 = getRes(expr2, false, false);
            if (!err.empty()) {
                return err;
            }
            
            for_->setRange({newExrp1, newExrp2});
            var->setRval(newExrp1);
            for_->setIter(var);

            auto res = analyseBody_(for_->body(), parDecls, args);
            if (!res.empty()) {
                return res;
            }
            parDecls->removeDecl(var);
        } else if (auto asg = std::dynamic_pointer_cast<node::Assign>(stm)) {
            auto newLval = getRes(asg->lval(), true, false);
            if (!err.empty()) {
                return err;
            }
            asg->setLval(newLval);

            auto newRval = getRes(asg->rval(), false, false);
            if (!err.empty()) {
                return err;
            }
            asg->setRval(newRval);
        } else if (auto ret = std::dynamic_pointer_cast<node::Return>(stm)) {
            auto retVal = ret->retVal();
            if (retVal) {
                auto newRetVAl = getRes(retVal, false, false);
                if (!err.empty()) {
                    return err;
                }
                ret->setRetVal(newRetVAl);
            }
        } else if (auto call = std::dynamic_pointer_cast<node::MBCall>(stm)) {
            auto newCall = getRes(call->call(), false, true);
            if (!err.empty()) {
                return err;
            }
            call->setCall(newCall);
            auto dotOp = std::dynamic_pointer_cast<node::DotOpExpr>(newCall);
            auto tail = dotOp->tail();
            
            if (auto tailCall = std::dynamic_pointer_cast<node::CallExpr>(tail)) {
                tailCall->setNoValue();
            } else {
                std::dynamic_pointer_cast<node::CallMethodExpr>(tail)->setNoValue();
            }
        }
    }

    return "";
}

std::pair<std::string, std::shared_ptr<node::IExpr>> 
LinkExprs::analyseOp_(std::shared_ptr<node::Op> op) {
    static attribute::QualifiedName name;

    auto getRes = [this](auto&& exprOrOP, auto&& qName) {
        if (auto op = std::dynamic_pointer_cast<node::Op>(exprOrOP)) {
            return analyseOp_(op);
        } else {
            return analyseExpr_(exprOrOP, qName);
        }
    };

    if (op->op() != node::OpType::DOT) {
        name.clear();
    }
    
    std::shared_ptr<node::IExpr> leftRes, rightRes;
    if (auto left = op->left()) {
        std::string err;
        std::tie(err, leftRes) = getRes(left, name);
        if (!err.empty()) {
            return {err, nullptr};
        }
        if (leftRes) {
            op->setLeft(leftRes);
            // обработка рекорда
            auto leftDotOps = std::dynamic_pointer_cast<node::DotOpExpr>(leftRes);
            std::shared_ptr<node::DotOpExpr> tail;
            if (leftDotOps){
                tail = leftDotOps->tail();
            }
        }
        if (op->op() != node::OpType::DOT) {
            name.clear();
        }
    } 

    if (auto right = op->right()) {
        auto leftDotOps = std::dynamic_pointer_cast<node::DotOpExpr>(leftRes);
        if (op->op() == node::OpType::DOT && leftDotOps && !leftDotOps->container()) {
            std::string res = "Incorrect qualified name using: ";
            res += name.toString('.');
            return {res, nullptr};
        } else if (leftDotOps) {
            auto tail = leftDotOps->tail();
            if (op->op() == node::OpType::DOT && tail->container() && 
                !std::dynamic_pointer_cast<node::PackNamePart>(tail)) 
            {
                auto res = analyseRecord_(leftDotOps, op->right());
                if (!res.empty()) {
                    return {res, nullptr};
                }
                name.clear();
                return {"", leftDotOps};
            }  
        }

        std::string err;
        std::tie(err, rightRes) = getRes(right, name);
        if (!err.empty()) {
            return {err, nullptr};
        }
        if (rightRes) {
            op->setRight(rightRes);
        }

        if (op->op() != node::OpType::DOT) {
            name.clear();
        }
    }

    if (op->op() == node::OpType::DOT) {
        auto resDotOp = leftRes ? 
                          std::dynamic_pointer_cast<node::DotOpExpr>(leftRes) :
                          std::dynamic_pointer_cast<node::DotOpExpr>(rightRes);
        auto rightDotOp = std::dynamic_pointer_cast<node::DotOpExpr>(rightRes);
        if (leftRes && rightRes) {
            resDotOp->setTail(rightDotOp);
        }

        return {"", resDotOp};
    }
    return {"", op};
}

std::string LinkExprs::analyseRecord_(
    std::shared_ptr<node::DotOpExpr> left, 
    std::shared_ptr<node::IExpr> right)
{
    if (auto dotOp = std::dynamic_pointer_cast<node::Op>(right)) {
        auto res = analyseRecord_(left, dotOp->left());
        if (!res.empty()) {
            return res;
        }
        return analyseRecord_(left, dotOp->right());
    }

    auto tail = left->tail();
    if (!tail->container()) {
        return "Incorrect use of a qualifying name";
    }

    auto ref = std::dynamic_pointer_cast<node::SuperclassReference>(tail->type());
    auto record = ref ? ref->cls()->record() : 
        std::dynamic_pointer_cast<node::RecordDecl>(tail->type());
    std::string name;
    tail->setNoAnalyse();
    std::vector<std::shared_ptr<node::IExpr>> args({tail});
    if (auto nameExpr = std::dynamic_pointer_cast<node::NameExpr>(right)) {
        name = nameExpr->name();
    } else {
        auto idxOrCallExpr = 
            std::dynamic_pointer_cast<node::CallOrIdxExpr>(right);
        name = std::dynamic_pointer_cast<node::NameExpr>(
            idxOrCallExpr->name())->name();
        auto&& exprArgs = idxOrCallExpr->args();
        args.insert(args.end(), exprArgs.begin(), exprArgs.end());
    }

    auto varCandidate = record->getVarDecl(name);

    std::vector<std::shared_ptr<node::IType>> argsTypes;
    std::transform(
        args.begin(), 
        args.end(), 
        std::inserter(argsTypes, argsTypes.end()), 
        [] (auto&& a) { 
            auto type = a->type();   
            auto ref = std::dynamic_pointer_cast<node::SuperclassReference>(type);
            if (ref) {
                type = ref->cls()->record();
            }
            return type;
        });

    std::shared_ptr<node::ProcBody> procCandidate;
    std::shared_ptr<node::FuncBody> funcCandidate;
    if (record->isTagged()) {
        auto cls = record->cls().lock();
        procCandidate = cls->containsMethod(name, argsTypes, true);
        funcCandidate = std::dynamic_pointer_cast<node::FuncBody>(
            cls->containsMethod(name, argsTypes, false));
    }

    if (varCandidate && (procCandidate || funcCandidate)) {
        return "Ambiguous:"
                " cannot determine whether" 
                " this is a method call or a field access: "
                + name;
    } else if (!varCandidate && !procCandidate && !funcCandidate) {
        return "Name cannot be resolved: " + name;
    }

    std::shared_ptr<node::DotOpExpr> rightDotOp;
    if (varCandidate) {
        rightDotOp = std::make_shared<node::GetVarExpr>(varCandidate);
    } else if (procCandidate || funcCandidate) {
        rightDotOp = std::make_shared<node::CallMethodExpr>(
            nullptr, procCandidate, funcCandidate, args);
    }

    left->setTail(rightDotOp);

    return "";
}

// получение конечной точки по пакетам (но не в рекордах)
std::pair<std::string, std::shared_ptr<node::IExpr>> 
LinkExprs::analyseExpr_(
    std::shared_ptr<node::IExpr> expr, 
    attribute::QualifiedName& base) 
{
    node::IDecl* par = nullptr;
    std::vector<std::shared_ptr<node::IExpr>> args;
    node::IDecl* requester = nullptr;

    if (auto nameExpr = std::dynamic_pointer_cast<node::NameExpr>(expr)) {
        base.push(nameExpr->name());
        par = dynamic_cast<node::IDecl*>(nameExpr->parent());
        requester = nameExpr->varDecl();
    } else if (auto callExpr = std::dynamic_pointer_cast<node::CallOrIdxExpr>(expr)) {
        // работа с image attr
        if (auto attr = std::dynamic_pointer_cast<node::AttributeExpr>(callExpr->name())) {
            node::SimpleType type;
            auto attrLeft = attr->attr().left();
            if (attrLeft.size() == 1) {
                auto imgTy = attrLeft.toString();
                bool err = false;
                type = imgTy == "integer" ? node::SimpleType::INTEGER :
                       imgTy == "boolean" ? node::SimpleType::BOOL    :
                       imgTy == "float"   ? node::SimpleType::FLOAT   :
                       (err = true, node::SimpleType::INTEGER);
                if (err) {
                    return {"unknown type for image" + imgTy, nullptr};
                }
                if (attr->attr().right() != "image") {
                    return {"All attributes except image" 
                            " are not available in" 
                            " this implementation" 
                            + attr->attr().right(), 
                            nullptr};
                }
                auto litType = std::make_shared<node::SimpleLiteralType>(type); 
                return {"", std::make_shared<node::ImageCallExpr>(callExpr->args()[0], litType)};
            } else {
                return {"Unknown argument type for image", nullptr};
            }
        }

        auto name = std::dynamic_pointer_cast<node::NameExpr>(callExpr->name());
        base.push(name->name());
        par = dynamic_cast<node::IDecl*>(callExpr->parent());
        args = callExpr->args();
    } else {
        return {"", expr};
    }

    if (par) {
        auto r = par->reachable(base, requester);
        std::shared_ptr<node::FuncBody> func;
        std::shared_ptr<node::ProcBody> proc;

        if (r.empty()) {
            std::stringstream ss;
            ss << "An unresolved name in an expression ";
            ss << base.toString('.');
            return {ss.str(), nullptr};
        }

        auto&& d = r.front()[0];
        if (auto var = std::dynamic_pointer_cast<node::VarDecl>(d)) {
            auto procPar = dynamic_cast<node::ProcBody*>(var->parent());
            auto curProc = dynamic_cast<node::ProcBody*>(expr->parent());
            if (procPar && curProc != procPar) {
                return {
                    "In this implementation, you cannot" 
                    " take variables from the" 
                    " external subprogram scope: " + base.toString('.'), nullptr};
            }
            auto type = var->type();
            if (auto alias = std::dynamic_pointer_cast<node::TypeAliasDecl>(type)) {
                type = alias->origin();
            }
            if (std::dynamic_pointer_cast<node::ArrayType>(type)
                || std::dynamic_pointer_cast<node::StringType>(type)) 
            {
                if (args.empty()) {
                    return {"", std::make_shared<node::GetVarExpr>(var)};
                }
                return {"", std::make_shared<node::GetArrElementExpr>(nullptr, var, args)};
            }
            if (!args.empty()) {
                std::string res = "The call is not a subprogram ";
                res += base.toString('.') + var->name();
                return {res, nullptr};
            }
            return {"", std::make_shared<node::GetVarExpr>(var)};
        } else if (auto pack = std::dynamic_pointer_cast<node::PackDecl>(d)) {
            if (!args.empty()) {
                std::string res = "Incorrect qualified name using: ";
                res += base.toString('.') + pack->name();
                return {res, nullptr};
            }
            return {"", std::make_shared<node::PackNamePart>(pack)};
        }

        auto eqArgs = [] (auto&& subp, auto&& args) {
            auto subpArgs = subp->params();
            if (subpArgs.size() != args.size()) {
                return false;
            }

            std::vector<std::shared_ptr<node::IType>> argsTypes;
            std::transform(args.begin(), args.end(), 
                        std::inserter(argsTypes, argsTypes.end()), 
                        [] (auto&& a) { return a->type(); });
            
            std::vector<std::shared_ptr<node::IType>> subpArgsTypes;
            std::transform(subpArgs.begin(), subpArgs.end(), 
                std::inserter(subpArgsTypes, subpArgsTypes.end()), 
                [] (auto&& a) { return a->type(); });


            for (int i = 0; i < args.size(); ++i) {
                auto&& subpArgType = subpArgsTypes[i];
                auto&& argType = argsTypes[i];
                auto ref = std::dynamic_pointer_cast<node::SuperclassReference>(subpArgType);
                auto rec = std::dynamic_pointer_cast<node::RecordDecl>(argType);
                if (ref && rec) {
                    auto cls = rec->cls().lock();
                    if (cls && cls->isDerivedOf(ref->cls())) {
                        continue;
                    } 
                }

                if (!subpArgType->compare(argType)) {
                    return false;
                }
            } 
            
            return true;
        };

        for (auto&& space : r) {
            for (auto&& d : space) {
                if (auto f = std::dynamic_pointer_cast<node::FuncBody>(d)) {
                    if (!func && eqArgs(f, args)) { 
                        func = f;
                    } else if (eqArgs(f, args)) {
                        return {"An ambiguous call " + func->name() , nullptr};
                    }
                } else if (auto p = std::dynamic_pointer_cast<node::ProcBody>(d)) {
                    if (!proc && eqArgs(p, args)) {
                        proc = p;
                    } else if (eqArgs(p, args)) {
                        return {"An ambiguous call: " + proc->name(), nullptr};
                    }
                }
            }
            if (proc && func) {
                break;
            }
        }

        if (!func && !proc) {
            std::stringstream ss;
            ss << "An unresolved name in an expression ";
            ss << base.toString('.');
            return {ss.str(), nullptr};
        }
        if ((proc && proc->cls()) || (func && func->cls())) {
            return {"", std::make_shared<node::CallMethodExpr>(nullptr, proc, func, args)};
        }
        return {"", std::make_shared<node::CallExpr>(nullptr, proc, func, args)};
    }  
    std::string res = "An unresolved name in an expression ";
    res += base.toString('.');
    return {res, nullptr};
}

static std::string analyseDotOpExpr(std::shared_ptr<node::IExpr> leftOrRight) {
    if (!leftOrRight) return "";

    auto op = std::dynamic_pointer_cast<node::Op>(leftOrRight);
    if (
        (!std::dynamic_pointer_cast<node::NameExpr>(leftOrRight) && 
         !std::dynamic_pointer_cast<node::CallOrIdxExpr>(leftOrRight) &&
         !op) ||
         (op && op->op() != node::OpType::DOT) 
       ) 
    {
        return "Invalid operands for Dot Op";
    } 
    return "";
}

static std::string analyseOpExpr(std::shared_ptr<node::IExpr> leftOrRight) {
    if (auto call = std::dynamic_pointer_cast<node::CallOrIdxExpr>(leftOrRight)) {
        if (!std::dynamic_pointer_cast<node::NameExpr>(call->name())) {
            return "Using a subprogram call without" 
                   " a qualifying name is" 
                   " not supported in this implementation";
        }
    }
    return "";
}

std::string 
LinkExprs::analyseOpExprErr_(std::shared_ptr<node::IExpr> expr) {
    auto op = std::dynamic_pointer_cast<node::Op>(expr);
    if (!op) {
        return "";
    }

    if (op->op() == node::OpType::DOT) {
        auto res1 = analyseDotOpExpr(op->left());
        auto res2 = analyseDotOpExpr(op->right());
        if (!res1.empty() || !res2.empty()) {
            return res1.empty() ? res2 : res1;
        }
    }
    
    auto res1 = analyseOpExpr(op->left());
    auto res2 = analyseOpExpr(op->right());
    if (!res1.empty() || !res2.empty()) {
        return res1.empty() ? res2 : res1;
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

std::string LinkExprs::analyseArgsExpr_(std::shared_ptr<node::IExpr> expr) {
    if (auto callOrIdx = 
            std::dynamic_pointer_cast<node::CallOrIdxExpr>(expr)) 
    {
        auto&& args = callOrIdx->args();
        for (auto&& a : args) {
            auto o = std::dynamic_pointer_cast<node::Op>(a);
            if (o) {
                auto oRes = analyseOpExprErr_(o);
                if (!oRes.empty()) {
                    return oRes;
                }    
            }
            attribute::QualifiedName _;

            auto err2 = analyseArgsExpr_(a);
            if (!err2.empty()) {
                return err2;
            }
            
            auto [err, newArg] = o ? analyseOp_(o) : analyseExpr_(a, _); 
            if (!err.empty()) {
                return err;
            } 

            auto rhsErr = analyseInOutRvalLvalNoVal_({}, newArg, false, false);
            if (!rhsErr.empty()) {
                return rhsErr;
            }

            a = newArg;
        }
    } else if (auto op = std::dynamic_pointer_cast<node::Op>(expr)) {
        auto res = analyseArgsExpr_(op->left());
        if (!res.empty()) {
            return res;
        }
        return analyseArgsExpr_(op->right());
    }

    return "";
}

// a.b = 1; (CE если a не out)
// a.b = x; или a.b = f(x); (CE, если x не in)
// lhs - где находиться выражение в assign
// noValue - только для первого по влож. вызова
std::string LinkExprs::analyseInOutRvalLvalNoVal_(
    const std::vector<std::shared_ptr<node::VarDecl>>& args, 
    std::shared_ptr<node::IExpr> expr, bool lhs, bool noValue,  
    bool first
)
{       
    if (first && lhs) {
        auto dotOp = std::dynamic_pointer_cast<node::DotOpExpr>(expr);
        if (!dotOp || !dotOp->lhs()) {
            return "Assignment of an lvalue to a value";
        }
    }

    std::shared_ptr<node::Op> op;
    if ((op = std::dynamic_pointer_cast<node::Op>(expr))) 
    {
        auto res = analyseInOutRvalLvalNoVal_(args, op->left(), lhs, noValue, false);
        if (!res.empty()) {
            return res;
        }

        return analyseInOutRvalLvalNoVal_(args, op->right(), lhs, noValue, false);
    } 

    if (auto dotOp = std::dynamic_pointer_cast<node::DotOpExpr>(expr)) {
        // вызов, индексация, обращение к переменной
        auto call = std::dynamic_pointer_cast<node::CallExpr>(dotOp);
        auto callMethod = std::dynamic_pointer_cast<node::CallMethodExpr>(dotOp);
        auto getVar = std::dynamic_pointer_cast<node::GetVarExpr>(dotOp);
        auto idx = std::dynamic_pointer_cast<node::GetArrElementExpr>(dotOp);
        // проверка на out/in для lhs/rhs в assign 
        if (first && (getVar || idx)) {
            auto var = getVar ? getVar->var() : idx->arr();
            auto it = std::find(args.begin(), args.end(), var);
            if (it != args.end() && lhs && !noValue && !var->out()) {
                return "Assigning a non-out variable " + var->name();
            } else if (it != args.end() && !lhs && !noValue && !var->in()) {
                return "Reading a non-in variable " + var->name();
            }
        }

        if (first) {
            auto tail = dotOp->tail();
            call = std::dynamic_pointer_cast<node::CallExpr>(tail);
            callMethod = std::dynamic_pointer_cast<node::CallMethodExpr>(tail);
            getVar = std::dynamic_pointer_cast<node::GetVarExpr>(tail);
            idx = std::dynamic_pointer_cast<node::GetArrElementExpr>(tail);
            // проверка на noValue (должны быть колы с процедурами)
            if (noValue) {
                auto callProc = call ? call->proc() : nullptr; 
                auto callMethodProc = callMethod ? callMethod->proc() : nullptr;
                if (!callProc && !callMethodProc) {
                    return "Not a value expression is not a procedure call";  
                } 
            } else {
                // проверка на value при lhs/rhs 
                if (!(call && call->func()) && !(callMethod && callMethod->func()) &&
                    !getVar && !idx) 
                {
                    return "Calling a procedure" 
                           " or accessing a package" 
                           " in an expression with the value";  
                }
                // проверка на lhs выражение (при присваивании)
                if (lhs && !getVar && !idx) {
                    return "Assigning rval to an expression";
                }
            }
        }

        // проверка аргументов вызова или индексации
        std::vector<std::shared_ptr<node::IExpr>> callArgs;
        if (call) callArgs = call->params();
        else if (callMethod) callArgs = callMethod->params();
        for (auto&& a : callArgs) {
            if (!a->noAnalyse()) {
                auto res = analyseInOutRvalLvalNoVal_(args, a, false, false, false);
                if (!res.empty()) {
                   return res;
                }
            }
        }

        // проверка аргументов вызовов или индексаций справа
        auto r = dotOp->right();
        if (r) {
            auto res = 
                analyseInOutRvalLvalNoVal_(args, r, false, false, false);
            if (!res.empty()) {
                return res;
            }
        }

        // проверка на pure rval при lhs
        if (first && lhs) {
            auto cur = dotOp;
            do {
                if (std::dynamic_pointer_cast<node::CallExpr>(cur) ||
                    std::dynamic_pointer_cast<node::CallMethodExpr>(cur))
                {
                    return "Lhs pure rvalue";
                } 
                else if (std::dynamic_pointer_cast<node::GetArrElementExpr>(cur) || 
                        std::dynamic_pointer_cast<node::GetVarExpr>(cur))
                {
                    break;
                }
            } while (cur = cur->right());
        }

    // проверка аргументов image
    // проверка что не lhs
    // проверка при noValue
    } else if (auto img = std::dynamic_pointer_cast<node::ImageCallExpr>(expr)) {
        if (lhs) {
            return "Assigning rval to an expression";
        }
        if (noValue) {
            return "Not a value expression is not a procedure call";
        }
        return analyseInOutRvalLvalNoVal_(
            args, img->param(), false, false, false);
    } else if (noValue) { // проверка на експр как стм
        return "Not a value expression is not a procedure call";  
    } 

    return "";
}

// TypeCheck
std::string TypeCheck::analyse(
        const std::vector<std::shared_ptr<mdl::Module>>& program)
{
    for (auto&& mod : program | std::views::drop(1)) {
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
TypeCheck::analyseContainer_(std::shared_ptr<node::IDecl> decl) {
    std::vector<std::shared_ptr<node::VarDecl>> args;
    std::shared_ptr<node::Body> body;
    std::shared_ptr<node::DeclArea> decls;

    if (auto proc = std::dynamic_pointer_cast<node::ProcBody>(decl)) {
        args = proc->params();
        body = proc->body();
        decls = proc->decls();
    } else if (auto pack = std::dynamic_pointer_cast<node::PackDecl>(decl)) {
        decls = pack->decls();
    } else if (auto record = std::dynamic_pointer_cast<node::RecordDecl>(decl)) {
        decls = record->decls();
    } else {
        return "";
    }

    if (body) {
        auto res = analyseBody_(body);
        if (!res.empty()) {
            return res;
        }
    }

    for (auto&& d : *decls) {
        if (auto var = std::dynamic_pointer_cast<node::VarDecl>(d)) {
            auto rhs = var->rval();
            if (rhs) {
                auto varType = var->type();
                if (!rhs->compareTypes(varType)) {
                    return "Assignment of different types in var decl: "
                           + var->name();
                }
            }
        } else {
            auto res = analyseContainer_(d);
            if (!res.empty()) {
                return res;
            }
        }
    }

    return "";
}

std::string 
TypeCheck::analyseBody_(std::shared_ptr<node::Body> body) {
    if (!body) {
        return "";
    }

    static auto BOOL_TY = 
        std::make_shared<node::SimpleLiteralType>(node::SimpleType::BOOL);
    static auto INT_TY = 
        std::make_shared<node::SimpleLiteralType>(node::SimpleType::INTEGER);

    for (auto&& stm : *body) {
        if (auto if_ = std::dynamic_pointer_cast<node::If>(stm)) {
            auto cond = if_->cond();
            if (!cond->compareTypes(BOOL_TY)) {
                return "The if condition expects a BOOLEAN expression";
            }
            
            auto bodyRes = analyseBody_(if_->body());
            if (!bodyRes.empty()) {
                return bodyRes;
            }

            auto elseRes = analyseBody_(if_->bodyElse());
            if (!elseRes.empty()) {
                return elseRes;
            }

            for (auto&& [cond, body] : if_->elsifs()) {
                if (!cond->compareTypes(BOOL_TY)) {
                    return "The if condition expects a Boolean expression";
                }
                auto bodyRes = analyseBody_(body);
                if (!bodyRes.empty()) {
                    return bodyRes;
                }
            }

        } else if (auto while_ = std::dynamic_pointer_cast<node::While>(stm)) {
            auto cond = while_->cond();
            if (!cond->compareTypes(BOOL_TY)) {
                return "The if condition expects a BOOLEAN expression";
            }

            auto bodyRes = analyseBody_(while_->body());
            if (!bodyRes.empty()) {
                return bodyRes;
            }
        } else if (auto for_ = std::dynamic_pointer_cast<node::For>(stm)) {
            auto [expr1, expr2] = for_->range();

            if (!expr1->compareTypes(INT_TY) || !expr2->compareTypes(INT_TY)) {
                return "The range boundaries for must be of type Integer";
            }

            auto bodyRes = analyseBody_(for_->body());
            if (!bodyRes.empty()) {
                return bodyRes;
            }
        } else if (auto asg = std::dynamic_pointer_cast<node::Assign>(stm)) {
            auto lhs = asg->lval();
            auto rhs = asg->rval();
            if (!rhs->compareTypes(lhs->type())) {
                return "Assignment between expressions of different types";
            }
        } else if (auto ret = std::dynamic_pointer_cast<node::Return>(stm)) {
            auto func = dynamic_cast<node::FuncBody*>(ret->parent());
            auto proc = dynamic_cast<node::ProcBody*>(ret->parent());
            if (!func && !proc) {
                throw std::logic_error("No func or proc return parent");
            }

            if (func) {
                auto funcType = func->retType();
                std::shared_ptr<node::IExpr> retVal;
                if (!(retVal = ret->retVal()) || !retVal->type()->compare(funcType)) {
                    return "Different types of return and functions";
                }
            } else {
                if (ret->retVal()) {
                    return "The return in procedure has a value";
                }
            }
        } 
    }
    return  "";
}

// QualifiedNameSet
std::string QualifiedNameSet::analyse(
        const std::vector<std::shared_ptr<mdl::Module>>& program)
{
    for (auto&& mod : program | std::views::drop(1)) {
        auto unit = mod->unit().lock();
        auto space = 
                std::dynamic_pointer_cast<node::GlobalSpace>(unit);
        analyseContainer_(space->unit(), mod->name());
    }
    return ISemanticsPart::analyseNext(program);
}


void QualifiedNameSet::analyseContainer_(
    std::shared_ptr<node::IDecl> decl, 
    attribute::QualifiedName name) 
{
    name.push(decl->name());
    decl->setFullName(name);
    std::shared_ptr<node::DeclArea> decls;
    if (auto pack = std::dynamic_pointer_cast<node::PackDecl>(decl)) {
        decls = pack->decls();
    } else if (auto sub = std::dynamic_pointer_cast<node::ProcBody>(decl)) {
        decls = sub->decls();
    }
    if (decls) {
        for (auto&& d : *decls) {
            analyseContainer_(d, name);
        }
    }

}

            
} // semantics_part
