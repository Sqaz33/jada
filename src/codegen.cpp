#include "codegen.hpp"

#include <fstream>

namespace codegen::java_bytecode_codegen {

JavaBCCodegen::JavaBCCodegen(
    std::uint16_t majorV,
    std::uint16_t minorV) :
    majorV_(majorV)
    , minorV_(minorV)
{}

void JavaBCCodegen::printClass( 
    jvm_class::SharedPtrJVMClass cls) const 
{
    std::fstream f(cls->simpleName() + ".class");
    cls->printBytes(f);
}

jvm_class::SharedPtrJVMClass 
JavaBCCodegen::createClass(
    const attribute::QualifiedName& name) 
{
    auto cls = std::make_shared<
                jvm_class::JVMClass>(
                    name, majorV_, minorV_);
    clss_.push_back(cls);
    return cls;
}

} // namespace codegen::java_bytecode_codegen