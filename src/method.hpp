#pragma once

#include <memory>

#include "class_member.hpp"
#include "jvm_attribute.hpp"
#include "descriptor.hpp"

namespace class_member {
class JVMClassMethod : private IJvmClassMember {
public:
    JVMClassMethod(  // TODO: class, class-name
        const std::string& name, 
        descriptor::JvmMethodDescriptor type,   // utf8 - descriptor constant_pool idx
        constant_pool::SharedPtrJvmCP cp);

public:
    void printBytes(std::ostream& out) const;
    void addFlag(codegen::AccessFlag flag);
    void addAttr(UniquePtrAttr attr);

public:
    void insertInstr(bb::SharedPtrBB bb, instr::Instr instr);
    bb::SharedPtrBB createBB();
    void insertBranch(
        bb::SharedPtrBB from, 
        instr::Instr instr, 
        bb::SharedPtrBB to);
    
    // TODO insert virtual/static call 
private:
    std::shared_ptr<jvm_attribute::CodeAttr> code_;
    std::uint16_t selfMethodRef_;
};

} // namespace class_member