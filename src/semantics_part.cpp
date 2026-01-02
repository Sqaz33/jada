#include "semantics_part.hpp"

#include <sstream>
#include <ranges>
#include <set>
#include <iterator>
#include <algorithm>

#include "node.hpp"

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
            if (name.size() != 1) {
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

} // semantics_part
