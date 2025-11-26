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
        std::weak_ptr<jvm_class::JVMClass> cls);

    JVMClassMethod(const JVMClassMethod&) = delete;
    JVMClassMethod(JVMClassMethod&&) = default;

public:
    void printBytes(std::ostream& out) const;

public:
    void addFlag(codegen::AccessFlag flag);
    using IJVMClassMember::addAttr;

public:
    bb::SharedPtrBB createBB();

    void createLocalInt(const std::string& name);
    void createLocalBoolean(const std::string& name);
    void createLocalDouble(const std::string& name);
    void createLocalFloat(const std::string& name);
    // void create


    // void insertInstr(bb::SharedPtrBB bb, instr::Instr instr);
    // void insertBranch(
    //     bb::SharedPtrBB from, 
    //     instr::Instr instr, 
    //     bb::SharedPtrBB to);

    // TODO insert virtual/static call clone method ref on call in other class 
    // TODO insert (static) get/set field  
    // TODO: следит за вызовами в разных класса
    // TODO: правильная вставка интсрукции (куча методов)

private:
    std::shared_ptr<jvm_attribute::CodeAttr> code_;
    std::uint16_t selfMethodRef_;
    std::weak_ptr<jvm_class::JVMClass> selfClass_;
    std::vector<std::weak_ptr<jvm_class::JVMClass>> classes_;

    bool isStatic_ = false;
};

} // namespace class_member