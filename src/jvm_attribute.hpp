#pragma once 

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "constant_pool.hpp"
#include "codegen_enums.hpp"
#include "instruction.hpp"
#include "basic_block.hpp"

namespace jvm_attribute {
    
class IAttribute {
public:
    IAttribute(const std::string& name, 
               constant_pool::SharedPtrJVMCP cp); 

    virtual ~IAttribute() = default;
    
    virtual const std::string& name() const noexcept = 0;

public:
    virtual void printBytes(std::ostream& out) const = 0;

protected:
    void setAttrLent(std::uint32_t len);

private:
    std::uint16_t name_;
    std::uint32_t attrLen_ = 0;
};

} // namespace jvm_attribute

namespace jvm_attribute {

class CodeAttr : 
    public IAttribute 
    , public std::enable_shared_from_this<CodeAttr>
{ 
public:
    CodeAttr(constant_pool::SharedPtrJVMCP cp);

public:
    bb::SharedPtrBB createBB();

    void createLocal(const std::string& name, 
        std::uint16_t size);
    std::uint16_t localIdx(const std::string& name);

    void insertInstr(bb::SharedPtrBB bb, instr::Instr instr);
    void insertBranch(
        bb::SharedPtrBB from, 
        instr::OpCode op, 
        bb::SharedPtrBB to); 
    void instertInstrWithLocal(
        bb::SharedPtrBB bb, 
        instr::OpCode op, 
        const std::string& name,
        const std::vector<std::uint8_t>& bytes = {});

    const std::string& name() const noexcept override;

public:
    void printBytes(std::ostream& out) const override;

private:
    void calcBBAddr_();
    void calcSelfLen_();
    
    std::uint16_t maxStack_() const;
    std::uint16_t maxLocals_() const;
    std::uint32_t codeLen_() const;
    std::uint32_t selfLen_() const;

    void checBBThenThrow_(bb::SharedPtrBB bb);
    
private:
    std::vector<bb::SharedPtrBB> code_;
    // TODO: exception table 
    // TODO: attrs

    // class internal
    std::map<std::string, 
        std::pair<std::uint16_t, std::uint16_t>> locals_;
    static const std::string name_;
};

} // namespace jvm_attribute
