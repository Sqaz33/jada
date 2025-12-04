#pragma once

#include <cstdint>
#include <ostream>
#include <vector>

#include "opcode.hpp"

namespace instr {

class Instr {
public:
    Instr(OpCode op, bool isBranch = false);
    
public:
    std::uint32_t idx() const noexcept;
    void setIdx(std::uint32_t idx) noexcept;  
    void printBytes(std::ostream& out) const;
    std::uint32_t len() const noexcept;
    OpCode opCode() const noexcept;
    bool isBranch() const noexcept;

    void pushByte(std::uint8_t byte);
    void pushTwoBytes(std::uint16_t bytes);
    void pushFourBytes(std::uint32_t bytes);

private:
    // byte structure
    const OpCode op_;
    std::vector<std::uint8_t> bytes_;

    // class internals
    bool isBranch_;
    std::uint32_t idx_;
};

} // namespace instr