#include "jvm_attribute.hpp"

#include "bits_utility.hpp"

namespace jvm_attribute {

IAttribute::IAttribute(const std::string& name, 
                       constant_pool::SharedPtrJVMCP cp) 
{
    auto [f, idx] = cp->getUtf8NameIdx(name);
    if (f) {
        name_ = idx;
    } else {
        name_ = cp->addUtf8Name(name);
    }
}

void IAttribute::setAttrLent(std::uint32_t len) {
    attrLen_ = len;
}

void IAttribute::printBytes(std::ostream& out) const {
    utility::printBytes(out, utility::reverse(name_));
    utility::printBytes(out, utility::reverse(attrLen_));
}

} // namespace jvm_attribute
