#include "basic_block.hpp"

#include <limits>
#include <cmath>

namespace {
std::uint16_t opStackSize(instr::OpCode op) {
    using O = instr::OpCode;

    switch (op) {
        case O::iconst_m1: case O::iconst_0: case O::iconst_1:
        case O::iconst_2:  case O::iconst_3: case O::iconst_4:
        case O::iconst_5:  case O::fconst_0: case O::fconst_1:
        case O::fconst_2:  case O::aconst_null:
            return 1;

        case O::lconst_0: case O::lconst_1:
        case O::dconst_0: case O::dconst_1:
            return 2;  

        case O::iload: case O::iload_0: case O::iload_1:
        case O::iload_2: case O::iload_3:
        case O::fload: case O::fload_0: case O::fload_1:
        case O::fload_2: case O::fload_3:
        case O::aload: case O::aload_0: case O::aload_1:
        case O::aload_2: case O::aload_3:
            return 1;

        case O::lload: case O::lload_0: case O::lload_1:
        case O::lload_2: case O::lload_3:
        case O::dload: case O::dload_0: case O::dload_1:
        case O::dload_2: case O::dload_3:
            return 2;

        case O::getstatic:
            return 2;

        case O::getfield:
            return 2;

        case O::new_:
            return 1;

        case O::dup:
            return 1;

        case O::dup2:
            return 2;

        case O::invokevirtual:
        case O::invokespecial:
        case O::invokestatic:
        case O::invokeinterface:
            return 2;

        case O::ldc: case O::ldc_w: 
            return 1;
        case O::ldc2_w:
            return 2;

        default:
            return 0;
    }
}

} // namespace

namespace bb {

BasicBlock::BasicBlock(        
        int id, 
        std::weak_ptr<jvm_attribute::CodeAttr> code) :
    id_(id)
    , code_(code)
{}

void BasicBlock::printBytes(std::ostream& out) const {
    int brIdx = 0;
    for (auto&& i : instrs_) {
        if (i) {
            i->printBytes(out);
        } else {
            auto&& [op, bb] = branches_[brIdx++];
            instr::Instr ins(op);
            auto idxOld 
                = static_cast<std::int64_t>(startOpCodeIdx_);
            auto offset = static_cast<std::int32_t>(
                bb->startOpCodeIdx() - idxOld);
            if (instr::OpCode::goto_w == op ||
                instr::OpCode::jsr_w == op)
            {
                ins.pushFourBytes(offset);
            } 
            else 
            {
                if (std::abs(offset) >= 
                    std::numeric_limits<std::int16_t>::max())
                {
                    throw std::logic_error("For a four-byte opcode offset," 
                                           " an instruction that" 
                                           " accepts two bytes is used");
                }
                ins.pushTwoBytes(
                    static_cast<std::int16_t>(offset));
            }
            ins.printBytes(out); 
        }
    }
}

void BasicBlock::insertInstr(instr::Instr instr) {
    instrs_.emplace_back(new instr::Instr(std::move(instr)));
}

void BasicBlock::insertBranch(
    instr::OpCode op, bb::SharedPtrBB to)
{
    instrs_.emplace_back(nullptr);
    branches_.emplace_back(op, to);
}

std::uint32_t 
BasicBlock::startOpCodeIdx() const noexcept {
    return startOpCodeIdx_;
}

void BasicBlock::setStartOpCodeIdx(
    std::uint32_t idx) noexcept 
{ startOpCodeIdx_ = idx; }

std::weak_ptr<jvm_attribute::CodeAttr>
BasicBlock::codeAttr() {
    return code_;
}

std::uint32_t BasicBlock::len() const {
    std::uint32_t len = 0;
    for (auto&& i : instrs_) {
        if (i) {
            len += i->len();
        }
    } 
    for (auto [_, bb] : branches_) {
        auto b4 = bb->startOpCodeIdx();
        len += 1 + (b4 > std::numeric_limits<std::uint16_t>::max() 
                    ? 4 
                    : 2);
    }
    return len;
}

std::uint16_t BasicBlock::stackSize() const {
    std::uint16_t stack = 0;
    for (auto&& i : instrs_) {
        if (i) {
            stack += opStackSize(i->opCode());
        }
    } 
    for (auto&& [op, _] : branches_) {
        stack += opStackSize(op);
    } 
    return stack;
}

} // namespace bb