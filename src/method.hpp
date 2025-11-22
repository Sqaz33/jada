#pragma once

#include <memory>

#include "class_member.hpp"
#include "jvm_attribute.hpp"
#include "descriptor.hpp"
#include "jvm_class.hpp"

namespace class_member {

class JVMClassMethod : private IJVMClassMember {
public:
    JVMClassMethod(  
        const std::string& name,
        descriptor::JVMMethodDescriptor type,   
        std::shared_ptr<jvm_class::JVMClassFile> cls);

    JVMClassMethod(const JVMClassMethod&) = delete;

    JVMClassMethod(JVMClassMethod&&) = default;

public:
    void printBytes(std::ostream& out) const;

public:
    void addFlag(codegen::AccessFlag flag);
    void addAttr(UniquePtrAttr attr);

public:
    void insertInstr(bb::SharedPtrBB bb, instr::Instr instr);
    bb::SharedPtrBB createBB();
    void insertBranch(
        bb::SharedPtrBB from, 
        instr::Instr instr, 
        bb::SharedPtrBB to);

    // TODO insert virtual/static call clone method ref on call in other class 
    // TODO insert (static) get/set field  
    // TODO: следит за вызовами в разных класса
    // TODO: правильная вставка интсрукции (куча методов)

private:
    std::shared_ptr<jvm_attribute::CodeAttr> code_;
    std::uint16_t selfMethodRef_;
    std::shared_ptr<jvm_class::JVMClassFile> class_;
};

} // namespace class_member