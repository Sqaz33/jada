#pragma once 

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "constant_pool.hpp"
#include "codegen_enums.hpp"
#include "instruction.hpp"

namespace bb {
    class BasicBlock;
    using SharedPtrBB = std::shared_ptr<BasicBlock>;
}

namespace jvm_attribute {
    
class IAttribute {
public:
    IAttribute(const std::string& name, 
               constant_pool::JVMConstantPool& jcp); 

    virtual ~IAttribute() = default;
    
    virtual const std::string& name() const noexcept = 0;

public:
    void increaseLen(std::uint32_t add) noexcept;
    
public:
    virtual void printBytes(std::ostream& out) const = 0;

private:
    std::uint32_t attrLen_;
    std::uint16_t name_;
};

} // namespace jvm_attribute

namespace jvm_attribute {

class CodeAttr : public IAttribute {
public:
    CodeAttr(constant_pool::JVMConstantPool& jcp);

public:
    void addInstr(instr::Instr instr);

    void increaseStack(std::uint32_t add) noexcept;
    void increaseLocals(std::uint32_t add) noexcept;

public:
    void printBytes(std::ostream& out) const override;

private:
    static const std::string name_; // = "Code"; // TODO

    std::uint16_t maxStack_;
    std::uint16_t maxLocals_;
    std::vector<instr::Instr> code_;
    std::uint16_t exceptionTableLen_ = 0;
    // TODO: exception table 
    std::uint16_t attrCount_ = 0;
    // TODO: attrs
};


} // namespace jvm_attribute
