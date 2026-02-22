#include "field.hpp"

#include "jvm_class.hpp"

namespace class_member {

JVMClassField::JVMClassField(
    const std::string& name,
    descriptor::JVMFieldDescriptor type,   
    std::weak_ptr<jvm_class::JVMClass> cls) :
    IJVMClassMember(
        cls.lock()->cp()->addUtf8Name(name), 
        cls.lock()->cp()->addFieldDescriptor(std::move(type)),
        true,
        cls.lock()->cp())
    , selfClass_(cls) 
    , name__(name)
    , type__(type)
{
    auto cp = cls.lock()->cp();
    auto nameNType = cp->addNameAndType(
            this->name(), this->type());
    fieldRef_ = cp->addFieldRef(
        cls.lock()->nameIdx(), nameNType);
}

std::uint16_t JVMClassField::selfClassRef() const noexcept {
    return fieldRef_;
}

jvm_class::SharedPtrJVMClass JVMClassField::cls() {
    return selfClass_.lock();
}

const std::string& 
JVMClassField::fieldName() const noexcept {
    return name__;
}

const descriptor::JVMFieldDescriptor&
JVMClassField::fieldType() const noexcept {
    return type__;
}

} // namespace class_member