#include "instruction.hpp"

#include "bits_utility.hpp"

namespace instr {

Instr::Instr(OpCode op) : op_(op) {}

void Instr::printBytes(std::ostream& out) const {
    auto op = static_cast<std::uint8_t>(op_);
    utility::printBytes(out, op);

    for (auto b : bytes_) {
        utility::printBytes(out, b);
    }
}

std::uint32_t Instr::len() const noexcept {
    auto sz 
        = static_cast<std::uint32_t>(bytes_.size());
    return 1 + sz;
}

OpCode Instr::opCode() const noexcept {
    return op_;
}

void Instr::pushByte(std::uint8_t byte) {
    bytes_.push_back(byte);
}

void Instr::pushTwoBytes(std::uint16_t bytes) {
    std::uint16_t mask = 0xff;
    bytes_.push_back((bytes >> 8) & mask);
    bytes_.push_back(bytes & mask);
}

void Instr::pushFourBytes(std::uint32_t bytes) {
    std::uint16_t mask = 0xff;
    bytes_.push_back((bytes >> 24) & mask);
    bytes_.push_back((bytes >> 16) & mask);
    bytes_.push_back((bytes >> 8) & mask);
    bytes_.push_back(bytes & mask);
}

} // namespace instr