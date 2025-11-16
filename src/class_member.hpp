#pragma once 

#include "codegen_enums.hpp"
#include "attribute.hpp"

namespace class_members {

class IJvmClassMember {
    virtual ~IJvmClassMember() = default;

private:
    codegen::AccessFlag accf_;
    std::uint16_t name_; 
    std::uint16_t desc_;
};

} // namespace jvm_class_members