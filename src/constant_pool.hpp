#pragma once 

#include "constant.hpp"

#include <utility>
#include <vector>
#include <map>

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
        const descriptor::JvmFieldDescriptor& descr);
    std::uint16_t addMethodDescriptor(
        const descriptor::JvmMethodDescriptor& descr);

    std::uint16_t addUtf8Name(const std::string& name);
    std::pair<bool, std::uint16_t> getUtf8NameIdx(
        const std::string& name);

public:
    void printBytes(std::ostream& out) const;
    
private:
    std::vector<std::unique_ptr<constant::IConstant>> consts_;
    std::map<std::string, std::uint16_t> named_;
};

} // namespace constant_pool