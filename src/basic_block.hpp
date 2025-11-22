#pragma once

#include <vector>
#include <memory>

#include "instruction.hpp"

namespace jvm_attribute {
    class IAttribute;
}

namespace bb {

class BasicBlock {
public:
    friend class jvm_attribute::IAttribute;
    
private:
    BasicBlock(int id, std::uint16_t startOpCodeIdx);

    void printBytes(std::ostream& out) const;

    void insertInstr(instr::Instr instr);
    void insertBranch(instr::OPCode op, 
                      std::shared_ptr<BasicBlock> to); // TODO: мб бренчи должны иметь боольше индексов
    

    std::uint16_t startOpCodeIdx() const noexcept;
    std::uint16_t len() const;
    std::uint16_t stackSize() const;
    int id() const noexcept;

private:
    std::uint16_t startOpCodeIdx_;
    std::vector<instr::Instr> instrs_;
    int id_;
};

using SharedPtrBB = std::shared_ptr<BasicBlock>;

} // namespace bb