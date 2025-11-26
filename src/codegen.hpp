// TODO: exception
// TODO: enum

#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <memory>

#include "constant_pool.hpp"

namespace codegen {

inline namespace java_bytecode_codegen {

using JVMClass  = std::shared_ptr<detail__::JVMClass>;
using Class         = std::shared_ptr<detail__::Class>;
using Method        = std::shared_ptr<detail__::Method>;

class JavaBCCodegen {
public:
    void printClassFile(JVMClass mdl, std::ostream& out) const;

private:
    std::vector<JVMClass> modules;
};

} // namespace java_bytecode_codegen

} // namespace codegen