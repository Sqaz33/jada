// TODO: check for int constant

#pragma once 

#include "constant.hpp"

#include <utility>
#include <vector>
#include <map>
#include <memory>

namespace constant_pool {

class JVMConstantPool {
public:
    JVMConstantPool();

public:
    std::uint16_t addUtf8(const std::string& text);
    std::uint16_t addInteger(int numb);
    std::uint16_t addFloat(float numb);
    std::uint16_t addString(std::uint16_t utf8);
    std::uint16_t addClass(std::uint16_t name);
    std::uint16_t addFieldRef(std::uint16_t cls, std::uint16_t nameNType);
    std::uint16_t addMehodRef(std::uint16_t cls, std::uint16_t nameNType);
    std::uint16_t addNameAndType(std::uint16_t name, std::uint16_t descr); 

    std::uint16_t addFieldDescriptor(
        const descriptor::JVMFieldDescriptor& descr);
    std::uint16_t addMethodDescriptor(
        const descriptor::JVMMethodDescriptor& descr);

    std::uint16_t addUtf8Name(const std::string& name);
    std::pair<bool, std::uint16_t> getUtf8NameIdx(
        const std::string& name);

public:void printBytes(std::ostream& out) const;
    
private:
    std::vector<std::unique_ptr<constant::IConstant>> consts_;
    std::map<std::string, std::uint16_t> named_;
};

using SharedPtrJVMCP = std::shared_ptr<JVMConstantPool>;

} // namespace constant_pool