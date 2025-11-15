#include "constant_pool.hpp"

namespace constant_pool {

JVMConstantPool::JVMConstantPool() : consts_(1) {}

std::uint16_t JVMConstantPool::addUtf8(const std::string& text) {
    auto c = std::make_unique<constant::Utf8>(text);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    return sz - 1;
}

std::uint16_t JVMConstantPool::addInteger(int numb) {
    auto c = std::make_unique<constant::Integer>(numb);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    return sz - 1;
}

std::uint16_t JVMConstantPool::addFloat(float numb) {
    auto c = std::make_unique<constant::Float>(numb);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    return sz - 1;
}

std::uint16_t JVMConstantPool::addString(std::uint16_t utf8) {
    auto c = std::make_unique<constant::String>(utf8);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    return sz - 1;
}

std::uint16_t JVMConstantPool::addClass(std::uint16_t name) {
    auto c = std::make_unique<constant::Class>(name);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    return sz - 1;
}

std::uint16_t JVMConstantPool::addFieldRef(
    std::uint16_t cls, std::uint16_t nameNType) 
{
    auto c = 
        std::make_unique<constant::Fieldref>(cls, nameNType);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    return sz - 1;
}

std::uint16_t JVMConstantPool::addMehodRef(
    std::uint16_t cls, std::uint16_t nameNType) 
{
    auto c = 
        std::make_unique<constant::Methodref>(cls, nameNType);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    return sz - 1;
}

std::uint16_t JVMConstantPool::addNameAndType(
    std::uint16_t name, std::uint16_t descr) 
{
    auto c = 
        std::make_unique<constant::NameAndType>(name, descr);
    consts_.emplace_back(std::move(c));
    int sz = static_cast<int>(consts_.size());
    return sz - 1;
}

} // namespace constant_pool