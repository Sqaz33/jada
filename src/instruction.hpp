#pragma once

#include <cstdint>
#include <ostream>
#include <vector>

#include "opcode.hpp"

namespace instr {

class Instr {
public:
    Instr(OpCode op);
    Instr() = default;

public:
    void printBytes(std::ostream& out) const;
    std::uint32_t len() const noexcept;
    OpCode opCode() const noexcept;

    void pushByte(std::uint8_t byte);
    void pushTwoBytes(std::uint16_t bytes);
    void pushFourBytes(std::uint32_t bytes);

private:
    // byte structure
    const OpCode op_;
    std::vector<std::uint8_t> bytes_;
};

} // namespace instr