#include "jvm_attribute.hpp"

#include "bits_utility.hpp"

namespace jvm_attribute {

CodeAttr::CodeAttr(constant_pool::SharedPtrJVMCP cp) : 
    IAttribute(name_, cp) 
{}

bb::SharedPtrBB CodeAttr::createBB() {
    code_.emplace_back(
        new bb::BasicBlock(
            code_.size(), shared_from_this()));
    return code_.back();
}

void CodeAttr::createLocal(
    const std::string& name, std::uint16_t size) 
{
    auto [preIdx, preSz] 
        = locals_.rbegin()->second;
    locals_[name] 
        = std::make_pair(preIdx + preSz, size);
    calcSelfLen_();
}

std::uint16_t CodeAttr::localIdx(const std::string& name) {
    auto it = locals_.find(name);
    if (it == locals_.end()) {
        throw std::logic_error(
            "There is no local variable");
    }
    return (it->second).first;
}

void CodeAttr::insertInstr(
    bb::SharedPtrBB bb, instr::Instr instr)
{  
    bb->insertInstr(std::move(instr));
}

void CodeAttr::insertBranch(
    bb::SharedPtrBB from, 
    instr::OpCode op, 
    bb::SharedPtrBB to)
{   
    if (to->codeAttr().lock() != from->codeAttr().lock()) {
        throw std::logic_error("Branching is not" 
                               " within a single function");
    }
    checBBThenThrow_(from);
    from->insertBranch(op, to);    
    calcSelfLen_();
    calcBBAddr_(); 
}

void CodeAttr::instertInstrWithLocal(
    bb::SharedPtrBB bb, 
    instr::OpCode op, 
    const std::string& name,
    const std::vector<std::uint8_t>& bytes)
{
    checBBThenThrow_(bb);
    auto it = locals_.find(name);
    if (it == locals_.end()) {
        throw std::logic_error(
            "There is no local variable");
    }
    auto [idx, _] = it->second;
    std::unique_ptr<instr::Instr> ins;
    if (idx > std::numeric_limits<std::uint8_t>::max()) {
        ins.reset(
            new instr::Instr(instr::OpCode::wide));
        ins->pushByte(
            static_cast<std::uint8_t>(op));
        ins->pushTwoBytes(idx);
    } else {
        ins.reset(
            new instr::Instr(op));
        ins->pushByte(
            static_cast<std::uint8_t>(op));
        ins->pushTwoBytes(
            static_cast<std::uint8_t>(idx));
    }
    for (auto b : bytes) {
        ins->pushByte(b);
    }
    bb->insertInstr(*ins);
    calcBBAddr_();
    calcSelfLen_();
}

const std::string& CodeAttr::name() const noexcept {
    return name_;
}

void CodeAttr::printBytes(std::ostream& out) const {
    IAttribute::printBytes(out);
    utility::printBytes(out, 
        utility::reverse(maxStack_()));
    utility::printBytes(out, 
        utility::reverse(maxLocals_()));
    utility::printBytes(out, 
        utility::reverse(codeLen_()));
    for (auto bb : code_) {
        bb->printBytes(out);
    }
    utility::printBytes(out, 0); // TODO: exception_table
    utility::printBytes(out, 0); // TODO: attribute_count
}

void CodeAttr::calcBBAddr_() {
    std::uint32_t idx = 0;
    for (auto bb : code_) {
        bb->setStartOpCodeIdx(idx);
        idx += bb->len();
    }
}

void CodeAttr::calcSelfLen_() {
    IAttribute::setAttrLent(selfLen_());
}

std::uint16_t CodeAttr::maxStack_() const {
    std::uint16_t stack = 0;
    for (auto bb : code_) {
        stack += bb->stackSize();
    }
    return stack;
}

std::uint16_t CodeAttr::maxLocals_() const {
    std::uint16_t locals = 0;
    for (auto&& pair : locals_) {
        auto&& [_, sz] = pair.second;
        locals += sz;
    }
    return locals;
}

std::uint32_t CodeAttr::codeLen_() const {
    std::uint32_t len = 0;
    for (auto bb : code_) {
        len += bb->len();
    }
    return len;
}

std::uint32_t CodeAttr::selfLen_() const {
    return 
        6   // init      
        + 2 // max_stack
        + 2 // max_locals
        + 4 // code_len
        + codeLen_()
        + 0  // exception_table
        + 0  // attribute_count
        + 0; // attribute_info
}

void CodeAttr::checBBThenThrow_(bb::SharedPtrBB bb) {
    if (bb->codeAttr().lock() != shared_from_this()) {
        throw std::logic_error("working with bb" 
                               " from another method");
    }
}  

const std::string CodeAttr::name_ =  "Code";
 
} // namespace jvm_attribute