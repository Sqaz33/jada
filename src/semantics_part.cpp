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
            auto&& name = with->name();
            if (!moduleNames.contains(name.first())) {
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
        [] (auto mdl) { return mdl->unit(); });

    for (auto mdl : program) {
        auto unit = mdl->unit().lock();
        auto gp = 
            std::make_shared<node::GlobalSpace>(unit);
        mdl->resetUnit(gp);
        auto [ok1, with] = addImportsPtrs_(mdl, units);
        if (!ok1) {
            auto name = with->name().toString();
            return "The module cannot be imported:"
                    + name;
        }
        auto [ok2, use] = addReduceImportPtrs_(mdl, units);
        if (!ok2) {
            auto name = use->name().toString();
            return "The name cannot be reduced:"
                    + name;
        }
    }

    return ISemanticsPart::analyseNext(program);
}

std::pair<bool, std::shared_ptr<node::With>> 
GlobalSpaceCreation::addImportsPtrs_(
    std::shared_ptr<mdl::Module> mdl,
    const std::vector<std::shared_ptr<node::IDecl>>& units)
{
    std::map<std::string, std::shared_ptr<node::IDecl>> map;
    std::transform(
        units.begin(), units.end(), 
        std::inserter(map, map.end()), 
        [] (auto u) { return std::make_pair(u->name(), u); } );
    
    for (auto w : mdl->with()) {
        auto name = w->name().toString('.');
        auto it = map.find(name);
        if (it == map.end()) {
            return {false, w};
        }
        auto unit = mdl->unit().lock();
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
    std::shared_ptr<mdl::Module> mdl,
    const std::vector<std::shared_ptr<node::IDecl>>& units)
{
    auto unit = mdl->unit().lock();
    std::map<std::string, std::shared_ptr<node::IDecl>> imports;
    auto space = 
            std::dynamic_pointer_cast<node::GlobalSpace>(unit);
    if (space) {
        auto imp = space->imports();
        std::transform(imp.begin(), imp.end(), 
                       std::inserter(imports, imports.end()), 
                       [] (auto i) { return std::make_pair(i->name(), i); });
    } else {
        throw std::logic_error(
            "Internal error in semantic_part.cpp");
    }

    for (auto u : mdl->use()) {
        auto name = u->name().toString('.');
        auto it = imports.find(name);
        if (it == imports.end()) {
            return {false, u};
        }
        if (auto pack 
                = std::dynamic_pointer_cast<node::PackDecl>(it->second)) 
        {
            auto begin = pack->decls()->begin();
            auto end = pack->decls()->end();
            for (; begin != end; ++begin) {
                space->addImport(*begin);
            }
        }
    }
    return {true, nullptr};
}

} // semantics_part
