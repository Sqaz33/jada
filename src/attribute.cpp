#include "attribute.hpp"

#include <iostream>
#include <sstream>

namespace attribute {

QualifiedName::QualifiedName(const std::string& base) :
    fullName_({base})
{}

void QualifiedName::push(const std::string& name) {
    fullName_.push_back(name);
}

bool QualifiedName::empty() const noexcept {
    return fullName_.empty();    
}

std::string QualifiedName::toSring() const {
    if (empty()) return "";

    std::stringstream ss;
    std::size_t lim = fullName_.empty() ? 
                        0 : fullName_.size() - 1;
    for (std::size_t i = 0; i < lim; ++i) {
        ss << fullName_[i] << '/';
    }
    ss << fullName_.back();
    return ss.str();
}

const std::string& QualifiedName::first() const noexcept {
    return fullName_.front();
}
const std::string& QualifiedName::last() const noexcept {
    return fullName_.back();
}

std::size_t QualifiedName::size() const noexcept {
    return fullName_.size();
}

Attribute::Attribute(QualifiedName left, const std::string& right) :
    left_(std::move(left))
    , right_(right)
{}

std::string Attribute::toString() const {
    if (left_.empty() || right_.empty()) return "";
    return left_.toSring() + '\'' + right_;
}

} // namespace attribute