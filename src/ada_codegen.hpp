#pragma once

#include "module.hpp"
#include "codegen.hpp"

namespace codegen {

void gen(std::vector<std::shared_ptr<mdl::Module>>& program);

extern jvm_class::SharedPtrJVMClass InnerSubprograms;
extern jvm_class::SharedPtrJVMClass StringBuiler; // TODO
extern jvm_class::SharedPtrJVMClass PrintStream; // TODO
extern jvm_class::SharedPtrJVMClass AtomicInteger; // TODO
extern jvm_class::SharedPtrJVMClass AdaUtility; // TODO ??
extern jvm_class::SharedPtrJVMClass JavaObject; // TODO ??

} // namespace codegen