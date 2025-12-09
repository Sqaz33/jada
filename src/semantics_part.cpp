#include "semantics_part.hpp"

#include "node.hpp"

namespace semantics_part {

// EntryPointCheck
std::string EntryPointCheck::analyse(
        std::vector<mdl::Module> program) 
{
    auto entry = program[0].unit().lock();
    if (!std::dynamic_pointer_cast<node::ProcDecl>(entry)) {
        return "The entry point should be a procedure";
    }
    return ISemanticsPart::analyseNext(program);
}

}
