#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <memory>

namespace codegen {

namespace impl__ { // don't look here

class Module;   
class Class;
class Method;

} // namespace impl__

inline namespace java_bytecode_codegen {

using Module  = std::shared_ptr<impl__::Module>;
using Class   = std::shared_ptr<impl__::Class>;
using Method  = std::shared_ptr<impl__::Method>;

enum class FundamentalTypes {
    INT = 0,
    FLOAT,
    BOOLEAN
};

class JavaBCCodegen {
public:
    void printClassFile(const Module mdl, std::ostream& out) const;

private:
    std::vector<Module> modules;
};

} // namespace java_bytecode_codegen

} // namespace codegen