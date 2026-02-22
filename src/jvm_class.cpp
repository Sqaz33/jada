#include "jvm_class.hpp"

#include "bits_utility.hpp"

namespace jvm_class {

JVMClass::JVMClass(
    const attribute::QualifiedName& name, 
    std::uint16_t majorV,
    std::uint16_t minorV) :
    name_(name.toString())
    , simpleName_(name.last())
    , minorV_(minorV)
    , majorV_(majorV)
    , cp_(new constant_pool::JVMConstantPool)
    , nameIdx_(cp_->addClass(name.toString()))
{}

void JVMClass::printBytes(std::ostream& out) const {
    utility::printBytes(out, 
        utility::reverse(0xCAFEBABE));

    utility::printBytes(out, 
        utility::reverse(minorV_));
    utility::printBytes(out, 
        utility::reverse(majorV_));

    utility::printBytes(out, 
        utility::reverse(cp_->size()));
    cp_->printBytes(out);
    
    utility::printBytes(out, 
        utility::reverse(accf_));
    
    utility::printBytes(out, 
        utility::reverse(nameIdx_));

    utility::printBytes(out, 
        utility::reverse(parentIdx_));

    utility::printBytes(out, std::uint16_t(0)); // interface count

    auto fieldsCount =
         static_cast<std::uint16_t>(fields_.size()); 
    utility::printBytes(out, 
        utility::reverse(fieldsCount));
    
    for (auto&& f : fields_) {
        f->printBytes(out);
    }
    
    auto methodsCount =
         static_cast<std::uint16_t>(methods_.size()); 
    utility::printBytes(out, 
        utility::reverse(methodsCount));
    
    for (auto&& m : methods_) {
        m->printBytes(out);
    }
    
    auto attrsCount =
         static_cast<std::uint16_t>(attrs_.size()); 
    utility::printBytes(out, 
        utility::reverse(attrsCount));

    for (auto&& a : attrs_) {
        a->printBytes(out);
    }
}

constant_pool::SharedPtrJVMCP JVMClass::cp() {
    return cp_;
}

const std::string& JVMClass::name() const noexcept {
    return name_;
}

std::uint16_t JVMClass::nameIdx() const noexcept {
    return nameIdx_;
}

void JVMClass::setParent(
    std::weak_ptr<JVMClass> par) 
{
    auto lock = par.lock();
    auto [ok, idx] = 
        cp_->getClassIdx(lock->name());    
    if (!ok) {
        idx = cp_->addClass(lock->name());
    }
    parentIdx_ = idx;
    parent_ = par;
}

void JVMClass::addAttr(
    std::shared_ptr<jvm_attribute::IAttribute> attr)
{
    attrs_.push_back(attr);
}

void JVMClass::addAccesFlag(
    codegen::AccessFlag accf) 
{
    accf_ |= static_cast<std::uint16_t>(accf);
}

const std::string& 
JVMClass:: simpleName() const noexcept {
    return simpleName_;
}

class_member::SharedPtrField 
JVMClass::addField( 
    const std::string& name,
descriptor::JVMFieldDescriptor type)
{
    auto field = 
        std::make_shared<
            class_member::JVMClassField>(
                name, std::move(type), slf());
    fields_.push_back(field);
    classNFields_[this][field.get()] = field->selfClassRef();
    return field;
}

class_member::SharedPtrMethod 
JVMClass::addMethod( 
    const std::string& name,
    descriptor::JVMMethodDescriptor type)
{
    auto method = 
        std::make_shared<
            class_member::JVMClassMethod>(
                name, std::move(type), slf());
    methods_.push_back(method);
    classNMethods_[this][method.get()] = method->selfClassRef();
    return method;
}

std::uint16_t JVMClass::methodRef(
    class_member::SharedPtrMethod method) 
{
    auto otherClsLock = method->cls();

    if (!classNMethods_[otherClsLock.get()].contains(method.get())) {   
        auto name = cp_->addUtf8Name(method->methodName());
        auto type = cp_->addMethodDescriptor(method->methodType());
        auto nameNType = cp_->addNameAndType(name, type);
        auto clsName = linkThisClassNOtherClass_(otherClsLock);

        classNMethods_[otherClsLock.get()][method.get()] = 
            cp_->addMehodRef(clsName, nameNType);
    }
    return classNMethods_[otherClsLock.get()][method.get()];
}

std::uint16_t JVMClass::fieldRef(
    class_member::SharedPtrField field) 
{
    auto otherClsLock = field->cls();

    if (!classNFields_[otherClsLock.get()].contains(field.get())) {   
        auto name = cp_->addUtf8Name(field->fieldName());
        auto type = cp_->addFieldDescriptor(field->fieldType());
        auto nameNType = cp_->addNameAndType(name, type);
        auto clsName = linkThisClassNOtherClass_(otherClsLock);

        classNFields_[otherClsLock.get()][field.get()] = 
            cp_->addFieldRef(clsName, nameNType);
    }
    return classNFields_[otherClsLock.get()][field.get()];
}

std::uint16_t JVMClass::className(
    jvm_class::SharedPtrJVMClass cls) 
{
    return linkThisClassNOtherClass_(cls);
}

std::uint16_t JVMClass::linkThisClassNOtherClass_(    
    jvm_class::SharedPtrJVMClass otherClass)
{
    if (this == otherClass.get()) {
        return nameIdx_;
    }

    auto&& [ok, otherClsName] = cp_->getClassIdx(otherClass->name());
    if (!ok) {
        otherClsName = cp_->addClass(otherClass->name());
    }

    return otherClsName;
}

} // namespace jvm_class 