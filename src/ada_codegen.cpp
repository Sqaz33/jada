#include "ada_codegen.hpp"

#include <ranges>

#include "node.hpp"

namespace codegen {

jvm_class::SharedPtrJVMClass InnerSubprograms(
    cg.createClass(attribute::QualifiedName("inner_subprograms")));

void gen(std::vector<std::shared_ptr<mdl::Module>>& program) {
    std::vector<std::shared_ptr<node::IDecl>> decls;

for (auto&& mod : program | std::views::drop(1)) {
        auto unit = mod->unit().lock();
        auto space = std::dynamic_pointer_cast<node::GlobalSpace>(unit);
        auto spaceUnit = space->unit();
        decls.push_back(spaceUnit);
    }

    for (auto&& d : decls) {
        d->pregen(nullptr, nullptr);
    }

    for (auto&& d : decls) {
        d->codegen(nullptr);
    }

    for (auto&& d : decls) {
        d->printClass();
    }

    cg.printClass(InnerSubprograms);
}

} // namespace codegen
