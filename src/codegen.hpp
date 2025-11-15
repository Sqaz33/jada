#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <memory>

#include "constant_pool.hpp"

namespace codegen {

inline namespace java_bytecode_codegen {

namespace detail__ { // don't look here

class JVMClassFile {
public:

    void printBytes(std::ostream& out) const;

private:
    constant_pool::JVMConstantPool jcp_;
    // TODO: fields 
    // TODO: methods
    std::string name_;
};

class Class;
class Method;

} // namespace detail__

using JVMClassFile  = std::shared_ptr<detail__::JVMClassFile>;
using Class         = std::shared_ptr<detail__::Class>;
using Method        = std::shared_ptr<detail__::Method>;

enum class FundamentalTypes {
    INT = 0,
    FLOAT,
    BOOLEAN
};

class JavaBCCodegen {
public:
    void printClassFile(JVMClassFile mdl, std::ostream& out) const;

private:
    std::vector<JVMClassFile> modules;
};

} // namespace java_bytecode_codegen

} // namespace codegen