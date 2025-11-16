#pragma once 

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "constant_pool.hpp"
#include "codegen_enums.hpp"
#include "instruction.hpp"

namespace class_member_attribute {

enum class AttributeName {
    CODE,

};
    
class IAttribute {
public:
    IAttribute(const std::string& name, 
               constant_pool::JVMConstantPool& jcp); 

    virtual ~IAttribute() = default;

public:
    void increaseLen(std::uint32_t add);
    
public:
    virtual void printBytes(std::ostream& out) const = 0;

private:
    std::uint32_t attrLen_;
    std::uint16_t name_;
};

} // namespace class_member_attribute


namespace class_member_attribute {

class CodeAttr : public IAttribute {
public:
    CodeAttr(constant_pool::JVMConstantPool& jcp);

public:
    void addInstr(instr::IInstr instr);

public:
    void printBytes(std::ostream& out) const override;

private:
    std::uint16_t maxStack_;
    std::uint16_t maxLocals_;
    std::vector<instr::IInstr> code_;
    std::uint16_t exceptionTableLen_ = 0;
    // TODO: exception table 
    std::uint16_t attrCount_ = 0;
    // TODO: attrs
};


} // namespace class_member_attribute
