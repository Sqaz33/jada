#include "field.hpp"

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

std::uint16_t JVMClassField::ref(
    std::weak_ptr<jvm_class::JVMClass> cls)
{
    linkWithClass_(cls);
    std::uint16_t ref;
    if (selfClass_.lock() == cls.lock()) {
        ref = fieldRef_;
    } else {
        ref = classes_[cls.lock().get()];
    }
    return ref;
}

void JVMClassField::linkWithClass_(
    std::weak_ptr<jvm_class::JVMClass> cls)
{
    auto otherClsLock = cls.lock();
    auto clsLock = selfClass_.lock();

    if (otherClsLock == clsLock) {
        return;
    }

    if (!classes_.contains(otherClsLock.get())) {   
        auto cp = clsLock->cp();
        auto name = cp->addUtf8Name(name__);
        auto type = cp->addFieldDescriptor(type__);
        auto nameNType = cp->addNameAndType(name, type);

        auto [ok, otherClsName] = cp->getUtf8NameIdx(
                                    otherClsLock->name());
        if (!ok) {
            otherClsName = 
                cp->addUtf8Name(otherClsLock->name());
        }

        classes_[otherClsLock.get()] = 
            cp->addMehodRef(otherClsName, nameNType);
    }
}

} // namespace class_member