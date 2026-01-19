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
            auto name = with->name().toString();
            std::stringstream ss;
            ss << mod->fileName();
            ss << ":";
            ss << "The module cannot be imported: ";
            ss << name;
            return ss.str();
        }
        auto [ok2, use] = addReduceImportPtrs_(mod, units);
        if (!ok2) {
            auto name = use->name().toString();
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
        auto&& params = proc->params();
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

} // semantics_part
