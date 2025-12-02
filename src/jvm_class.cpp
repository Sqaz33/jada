#include "jvm_class.hpp"

#include "bits_utility.hpp"

namespace jvm_class {

JVMClass::JVMClass(
    const attribute::QualifiedName& name, 
    std::uint16_t majorV,
    std::uint16_t minorV) :
    name_(name.toSring())
    , simpleName_(name.last())
    , majorV_(majorV)
    , minorV_(minorV)
    , cp_(new constant_pool::JVMConstantPool)
    , nameIdx_(cp_->addClass(name.toSring()))
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
    return method;
}

} // namespace jvm_class 