#pragma once 

#include "codegen_enums.hpp"
#include "attribute.hpp"

namespace class_members {

class IJvmClassMember {

private:
    codegen::AccessFlags accf_;
    std::uint16_t name_; 
    std::uint16_t desc_;
};

} // namespace field