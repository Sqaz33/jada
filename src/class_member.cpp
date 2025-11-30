#include "class_member.hpp"

#include <stdexcept>

#include "bits_utility.hpp"

namespace class_member {

void IJVMClassMember::printBytes(std::ostream& out) const {
    auto flags 
        = static_cast<std::uint16_t>(accf_);
    utility::printBytes(out, 
        utility::reverse(flags));

    utility::printBytes(out, 
        utility::reverse(name_));
    utility::printBytes(out, 
        utility::reverse(desc_));

    auto sz 
        = static_cast<std::uint16_t>(attrs_.size());
    utility::printBytes(out, 
        utility::reverse(sz));

    for (auto&& attr : attrs_) {
        attr->printBytes(out);
    }
}

static void checkFlagsThenThrow( 
    codegen::AccessFlag all, 
    codegen::AccessFlag add,
    bool method) 
{   
    using namespace codegen;
    auto intAll = static_cast<std::uint16_t>(all);
    auto intAdd = static_cast<std::uint16_t>(add);
    if (intAll & intAdd) {
        return;
    }

    if (method &&
        (add == AccessFlag::ACC_TRANSIENT ||
         add == AccessFlag::ACC_ENUM))
    {
        throw std::logic_error(
            "Incorrect flag is applied to the method");
    }

    auto pub = static_cast<std::uint16_t>(
        AccessFlag::ACC_PUBLIC);
    auto priv = static_cast<std::uint16_t>(
        AccessFlag::ACC_PRIVATE);
    auto prot = static_cast<std::uint16_t>(
        AccessFlag::ACC_PROTECTED);
    if (
        (intAll & pub ||
         intAll & priv || 
         intAll & prot) &&
        (intAdd == pub || 
         intAdd == priv || 
         intAdd == prot)
    )
    {
        throw std::logic_error(
            "An attempt to set the PUBLIC," 
            " PRIVATE, and PROTECTED flags simultaneously"
        );
    }
}

void IJVMClassMember::addFlag(codegen::AccessFlag flag) {    
    auto accf = static_cast<codegen::AccessFlag>(accf_);
    checkFlagsThenThrow(accf, flag, isMethod_);
    auto intFlag = static_cast<std::uint16_t>(flag);
    isStatic_ = isStatic_ || 
        codegen::AccessFlag::ACC_STATIC == flag;
    accf_ |= intFlag;
}

void IJVMClassMember::addAttr(
    IJVMClassMember::SharedPtrAttr attr) 
{   
    auto idx = cp_->addUtf8Name(attr->name());
    attrs_.emplace_back(attr);
}

bool IJVMClassMember::isStatic() const noexcept {
    return isStatic_;
}

std::uint16_t IJVMClassMember::name() const noexcept {
    return name_;
}

std::uint16_t IJVMClassMember::type() const noexcept {
    return desc_;
}

IJVMClassMember::IJVMClassMember(
    std::uint16_t name, 
    std::uint16_t desc,
    bool isMethod,
    constant_pool::SharedPtrJVMCP cp
) :
    name_(name)
    , desc_(desc)
    , isMethod_(isMethod)
    , cp_(cp)
{}

} // namespace class_members 