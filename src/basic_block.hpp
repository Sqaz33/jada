#pragma once

#include <vector>
#include <memory>

#include "instruction.hpp"

namespace jvm_attribute {
    class CodeAttr;
}

namespace bb {

// TODO: delete: следит за бренчами и стеком
class BasicBlock {
public:
    friend class jvm_attribute::CodeAttr;

    std::uint32_t startOpCodeIdx() const noexcept;
    
private:
    BasicBlock(
        int id, 
        std::weak_ptr<jvm_attribute::CodeAttr> code);

    void printBytes(std::ostream& out) const;

    void insertInstr(instr::Instr instr);
    void insertBranch(
        instr::OpCode op, 
        std::shared_ptr<BasicBlock> to); 

    void setStartOpCodeIdx(std::uint32_t idx) noexcept;

    std::weak_ptr<jvm_attribute::CodeAttr> codeAttr();

    std::uint32_t len() const;
    std::uint16_t stackSize() const;
    
    int id() const noexcept;

private:
    std::uint32_t startOpCodeIdx_;
    std::vector<std::unique_ptr<instr::Instr>> instrs_;
    std::weak_ptr<jvm_attribute::CodeAttr> code_;
    std::vector<std::pair<
                        instr::OpCode, 
                        std::shared_ptr<BasicBlock>>> branches_;
    int id_;
};

using SharedPtrBB = std::shared_ptr<BasicBlock>;

} // namespace bb