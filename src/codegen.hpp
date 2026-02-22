// TODO: exception
// TODO: enum

#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <memory>

#include "jvm_class.hpp"

namespace codegen {

inline namespace java_bytecode_codegen {

class JavaBCCodegen {
public:
    JavaBCCodegen(
        std::uint16_t majorV,
        std::uint16_t minorV);

public:
    void printClass( // -> cls_name.class file 
        jvm_class::SharedPtrJVMClass cls) const;

    jvm_class::SharedPtrJVMClass createClass(
        const attribute::QualifiedName& name);
    
private:
    std::vector<jvm_class::SharedPtrJVMClass> clss_;
    std::uint16_t majorV_;
    std::uint16_t minorV_;
};

} // namespace java_bytecode_codegen

extern JavaBCCodegen cg;

} // namespace codegen