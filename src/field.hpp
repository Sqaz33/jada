#pragma once

#include "class_member.hpp"

namespace jvm_class {

class JVMClass; 

using SharedPtrJVMClass = std::shared_ptr<JVMClass>;

} //namespace jvm_class 

namespace class_member {

class JVMClassField : private IJVMClassMember  {
public:
    JVMClassField(
        const std::string& name,
        descriptor::JVMFieldDescriptor type,   
        std::weak_ptr<jvm_class::JVMClass> cls);

    JVMClassField(const JVMClassField&) = delete;
    JVMClassField(JVMClassField&&) = default;

public:
    using IJVMClassMember::addFlag;
    using IJVMClassMember::addAttr;
    using IJVMClassMember::isStatic;
    using IJVMClassMember::printBytes;

public:
    std::uint16_t selfClassRef() const noexcept;
    jvm_class::SharedPtrJVMClass cls();

    const std::string& fieldName() const noexcept;
    const descriptor::JVMFieldDescriptor& fieldType() const noexcept; 

private:
    std::weak_ptr<jvm_class::JVMClass> selfClass_;
    std::uint16_t fieldRef_;
    std::string name__;
    descriptor::JVMFieldDescriptor type__;
};

using SharedPtrField = std::shared_ptr<JVMClassField>;

} // namespace class_member