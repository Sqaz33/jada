#pragma once

#include <vector>
#include <memory>

#include "instruction.hpp"
#include "jvm_attribute.hpp"

namespace jvm_attribute {
    class CodeAttr;
}
// TODO: delete: следит за бренчами и стеком
namespace bb {

class BasicBlock {
public:
    friend class jvm_attribute::CodeAttr;
    
private:
    BasicBlock(
        int id, 
        const jvm_attribute::CodeAttr* codeAttr);

    void printBytes(std::ostream& out) const;

    void insertInstr(instr::Instr instr);
    void insertBranch(instr::OpCode op, bb::SharedPtrBB to); 

    std::uint32_t startOpCodeIdx() const noexcept;
    void setStartOpCodeIdx(std::uint32_t idx) noexcept;

    const jvm_attribute::CodeAttr* codeAttr() const noexcept;

    std::uint32_t len() const;
    std::uint16_t stackSize() const;
    
    int id() const noexcept;

private:
    std::uint32_t startOpCodeIdx_;
    std::vector<std::unique_ptr<instr::Instr>> instrs_;
    const jvm_attribute::CodeAttr* const code_;
    std::vector<std::pair<
            instr::OpCode, bb::SharedPtrBB>> branches_;
    int id_;
};

using SharedPtrBB = std::shared_ptr<BasicBlock>;

} // namespace bb