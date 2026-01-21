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
    if (!std::dynamic_pointer_cast<node::ProcDecl>(unit) ||
         std::dynamic_pointer_cast<node::FuncDecl>(unit)) 
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
    std::vector<std::shared_ptr<node::IDecl>> units;
    std::transform(
        program.begin(), program.end(),
        std::inserter(units, units.end()),
        [] (auto&& mod) { return mod->unit().lock(); });

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
        auto [ok2, use] = addReduceImportPtrs_(mod, units);
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
GlobalSpaceCreation::addReduceImportPtrs_(
    std::shared_ptr<mdl::Module> mod,
    const std::vector<std::shared_ptr<node::IDecl>>& units)
{
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

// NameConflictCheck 
std::string NameConflictCheck::analyse(
    const std::vector<
        std::shared_ptr<mdl::Module>>& program)
{
    for (auto&& mod : program) {
        auto space = 
                std::dynamic_pointer_cast<node::GlobalSpace>(mod->unit().lock());
        if (space) {
            auto res = analyzeDecl_(space->unit());
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

std::string NameConflictCheck::analyzeDecl_(
    std::shared_ptr<node::IDecl> decl)
{
    if (std::dynamic_pointer_cast<node::VarDecl>(decl) || 
        std::dynamic_pointer_cast<node::TypeAliasDecl>(decl))  
    { return ""; }

    std::map<std::string, std::shared_ptr<node::IDecl>> nameNDecl;
    std::vector<std::shared_ptr<node::IDecl>> allDecls;
    if (auto proc = 
            std::dynamic_pointer_cast<node::ProcDecl>(decl)) 
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
        }
    }

    for (auto&& d : allDecls) {
        auto it = nameNDecl.find(d->name());
        if (it != nameNDecl.end() && 
            (!std::dynamic_pointer_cast<node::ProcDecl>(d) || 
             !std::dynamic_pointer_cast<node::ProcDecl>(it->second))) 
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
        auto res = analyzeDecl_(d);
        if (!res.empty()) {
            return res;
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
            auto res = analyzeContainer_(unit);
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
TypeNameToRealType::analyzeContainer_(
    std::shared_ptr<node::IDecl> decl)
{   
    std::shared_ptr<node::DeclArea> decls;
    if (auto proc = 
            std::dynamic_pointer_cast<node::ProcDecl>(decl)) 
    {
        for (auto&& par : proc->params()) {
            auto res = analyzeParam_(par);
            if (!res.empty()) {
                std::stringstream ss;
                ss << decl->name() << ':';
                ss << res;
                return ss.str();
            }
        }
        decls = proc->decls();
    } 
    else if (auto record
                = std::dynamic_pointer_cast<node::RecordDecl>(decl))
    {
        decls = record->decls();
    }
    else if (auto pack =
                std::dynamic_pointer_cast<node::PackDecl>(decl))
    {
        decls = pack->decls();
    }

    for (auto&& d : *decls) {
        if (std::dynamic_pointer_cast<node::TypeAliasDecl>(d) || 
            std::dynamic_pointer_cast<node::VarDecl>(d) ||
            std::dynamic_pointer_cast<node::RecordDecl>(d))
        {
            auto res = analyzeDecl_(d);
            if (!res.empty()) {
                std::stringstream ss;
                ss << decl->name() << ':';
                ss << res;
                return ss.str();
            }
        } 
        else 
        {
            auto res = analyzeContainer_(d);
            if (!res.empty()) {
                std::stringstream ss;
                ss << decl->name() << '.';
                ss << res;
                return ss.str();
            }
        }
    }
    return "";
}

std::string 
TypeNameToRealType::analyzeDecl_(
    std::shared_ptr<node::IDecl> decl)
{   
    std::shared_ptr<node::TypeAliasDecl> alias;
    std::shared_ptr<node::VarDecl> var;
    std::shared_ptr<node::TypeName> typeName;
    if (alias = 
            std::dynamic_pointer_cast<node::TypeAliasDecl>(decl)) 
    {
        auto origin = alias->origin();
        if (typeName = 
                std::dynamic_pointer_cast<node::TypeName>(origin));
        else if (auto arrayType = 
                    std::dynamic_pointer_cast<node::ArrayType>(origin))
        {
            return analyzeArrayType_(arrayType, 
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
            return analyzeArrayType_(arrayType, 
                dynamic_cast<node::IDecl*>(decl->parent()),
                decl);
        }
    }
    else if (auto record = 
                std::dynamic_pointer_cast<node::RecordDecl>(decl)) 
    {
        return analyseRecord_(record);
    }
    
    if (typeName) {
        if (!typeName->hasName()) {
            std::stringstream ss;
            ss << "It is forbidden to" 
                  " use superclass reference" 
                  " for aliases, vars, arrays: ";
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
TypeNameToRealType::analyzeArrayType_(
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
    std::shared_ptr<node::RecordDecl> decl)
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
                auto res = analyseRecord_(base);
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
            isBaseSet = true;
            decl->setBase(base);
        }
    }

    if (!isBaseSet) {
        std::stringstream ss;
        ss << "Unresolved name during inheritance: ";
        ss << "Record: " << decl->name();
        ss << "Base Name: " << baseName.toString('.');
        return ss.str();
    } 

    decl->setTagged();

    return "";
}

std::string 
TypeNameToRealType::analyzeParam_(
    std::shared_ptr<node::VarDecl> decl) 
{   
    auto type = decl->type();
    if (auto typeName = 
            std::dynamic_pointer_cast<node::TypeName>(type)) 
    {
        if (typeName->hasName()) {
            return analyzeDecl_(decl);
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
        auto res = analyzeContainer_(space->unit());
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
OverloadCheck::analyzeContainer_(
    std::shared_ptr<node::IDecl> decl)
{
    std::shared_ptr<node::DeclArea> decls;

    if (auto pack = std::dynamic_pointer_cast<node::PackDecl>(decl)) {
        decls = pack->decls();
    } else if (auto proc = std::dynamic_pointer_cast<node::ProcDecl>(decl)) {
        decls = proc->decls();
    }

    std::map<std::string, std::vector<std::shared_ptr<node::IDecl>>> nameNDecls;
    for (auto&& d : *decls) {
        nameNDecls[d->name()].push_back(d);
    }

    for (auto&& [_, decls] : nameNDecls) {
        if (decls.size() > 1) {
            for (std::size_t i = 0; i < decls.size() - 1; ++i) {
                auto&& d1 = decls[i];
                auto proc1 = 
                        std::dynamic_pointer_cast<node::ProcDecl>(d1);
                auto&& params1 = proc1->params();
                auto isFunc1 = std::dynamic_pointer_cast<node::FuncDecl>(proc1);
                for (std::size_t j = i + 1; j < decls.size(); ++j) {
                    auto&& d2 = decls[j];
                    auto proc2 = 
                        std::dynamic_pointer_cast<node::ProcDecl>(d2);
                    auto isFunc2 = std::dynamic_pointer_cast<node::FuncDecl>(proc2);
                    
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

    for (auto&& d : *decls) {
        if (std::dynamic_pointer_cast<node::PackDecl>(d) || 
            std::dynamic_pointer_cast<node::ProcDecl>(d))
        {
            auto res = analyzeContainer_(d);
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

} // semantics_part
