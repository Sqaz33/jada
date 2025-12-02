#include "constant_pool.hpp"

namespace constant_pool {

JVMConstantPool::JVMConstantPool() : consts_(1) { 
    consts_[0].reset(nullptr);
}

std::uint16_t JVMConstantPool::addUtf8(const std::string& text) {
    auto c = std::make_unique<constant::Utf8>(text);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    return static_cast<std::uint16_t>(sz - 1);
}

std::uint16_t JVMConstantPool::addInteger(int numb) {
    if (intCnst_.contains(numb)) return intCnst_[numb];
    auto c = std::make_unique<constant::Integer>(numb);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    int idx = static_cast<std::uint16_t>(sz - 1);
    intCnst_[numb] = idx;
    return idx;
}

std::uint16_t JVMConstantPool::addFloat(float numb) {
    if (floatCnst_.contains(numb)) return floatCnst_[numb];
    auto c = std::make_unique<constant::Float>(numb);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    int idx = static_cast<std::uint16_t>(sz - 1);
    floatCnst_[numb] = idx;
    return idx;
}

std::uint16_t JVMConstantPool::addDouble(double numb) {
    if (doubleCnst_.contains(numb)) return doubleCnst_[numb];
    auto c = std::make_unique<constant::Double>(numb);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    int idx = static_cast<std::uint16_t>(sz - 1);
    doubleCnst_[numb] = idx;
    return idx;
}

std::uint16_t JVMConstantPool::addLong(std::int64_t numb) {
    if (longCnst_.contains(numb)) return longCnst_[numb];
    auto c = std::make_unique<constant::Long>(numb);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    int idx = static_cast<std::uint16_t>(sz - 1);
    longCnst_[numb] = idx;
    return idx;
}

std::uint16_t JVMConstantPool::addString(const std::string& string) {
    if (stringCnst_.contains(string)) return stringCnst_[string];
    auto c = std::make_unique<constant::String>(
        addUtf8(string));
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    int idx = static_cast<std::uint16_t>(sz - 1);
    stringCnst_[string] = idx;
    return idx;
}

std::uint16_t JVMConstantPool::addClass(
    const std::string& name) 
{   if (classesCnst_.contains(name)) return classesCnst_[name];
    auto [ok, utf8Idx] = getUtf8NameIdx(name);
    if (!ok) {
        utf8Idx = addUtf8(name);
    }
    auto c = std::make_unique<constant::Class>(utf8Idx);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    auto idx = static_cast<std::uint16_t>(sz - 1);
    classesCnst_[name] = idx;
    return idx;
}

std::uint16_t JVMConstantPool::addFieldRef(
    std::uint16_t cls, std::uint16_t nameNType) 
{
    auto c = 
        std::make_unique<constant::Fieldref>(cls, nameNType);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    return static_cast<std::uint16_t>(sz - 1);
}

std::uint16_t JVMConstantPool::addMehodRef(
    std::uint16_t cls, std::uint16_t nameNType) 
{
    auto c = 
        std::make_unique<constant::Methodref>(cls, nameNType);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    return static_cast<std::uint16_t>(sz - 1);
}

std::uint16_t JVMConstantPool::addNameAndType(
    std::uint16_t name, std::uint16_t descr) 
{
    auto c = 
        std::make_unique<constant::NameAndType>(name, descr);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    return static_cast<std::uint16_t>(sz - 1);
}

std::uint16_t 
JVMConstantPool::addUtf8Name(const std::string& name) {
    auto c = std::make_unique<constant::Utf8>(name);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    int idx = static_cast<std::uint16_t>(sz - 1);
    named_[name] = idx;
    return idx;
}

std::uint16_t 
JVMConstantPool::addFieldDescriptor(
    const descriptor::JVMFieldDescriptor& descr) 
{
    return addUtf8(descr.toString());
}

std::uint16_t 
JVMConstantPool::addMethodDescriptor(
    const descriptor::JVMMethodDescriptor& descr) 
{
    return addUtf8(descr.toString());
}

std::pair<bool, std::uint16_t>
JVMConstantPool::getUtf8NameIdx(const std::string& name) {
    decltype(named_.begin()) it;
    if ((it = named_.find(name)) != named_.end()) {
        return {true, it->second};
    }

    return {false, 0};
}

std::pair<bool, std::uint16_t>
JVMConstantPool::getNumbConstIdx(double numb) {
    decltype(doubleCnst_.begin()) it;
    if ((it = doubleCnst_.find(numb)) != doubleCnst_.end()) {
        return {true, it->second};
    }

    return {false, 0};
}

std::pair<bool, std::uint16_t>
JVMConstantPool::getNumbConstIdx(float numb) {
    decltype(floatCnst_.begin()) it;
    if ((it = floatCnst_.find(numb)) != floatCnst_.end()) {
        return {true, it->second};
    }

    return {false, 0};
}

std::pair<bool, std::uint16_t>
JVMConstantPool::getNumbConstIdx(int numb) {
    decltype(intCnst_.begin()) it;
    if ((it = intCnst_.find(numb)) != intCnst_.end()) {
        return {true, it->second};
    }

    return {false, 0};
}

std::pair<bool, std::uint16_t>
JVMConstantPool::getNumbConstIdx(std::int64_t numb) {
    decltype(longCnst_.begin()) it;
    if ((it = longCnst_.find(numb)) != longCnst_.end()) {
        return {true, it->second};
    }

    return {false, 0};
}

std::pair<bool, std::uint16_t> 
JVMConstantPool::getStringIdx(
    const std::string& string) 
{
    decltype(stringCnst_.begin()) it;
    if ((it = stringCnst_.find(string)) != stringCnst_.end()) {
        return {true, it->second};
    }

    return {false, 0};
}


std::pair<bool, std::uint16_t>
JVMConstantPool::getClassIdx(
        const std::string& name) 
{
    decltype(classesCnst_.begin()) it;
    if ((it = classesCnst_.find(name)) != classesCnst_.end()) {
        return {true, it->second};
    }

    return {false, 0};
}

void JVMConstantPool::printBytes(
    std::ostream& out) const 
{
    for (auto&& c : consts_) {
        if (c) {
            c->printBytes(out);
        }
    }
}

std::uint16_t JVMConstantPool::size() const noexcept {
    return static_cast<std::uint16_t>(
        consts_.size());
}

} // namespace constant_pool