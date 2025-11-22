#pragma once

#include <cstdint>
#include <ostream>
#include <vector>

#include "opcode.hpp"

namespace instr {

class Instr {
public:
    Instr(OPCode op);

public:
    void printBytes(std::ostream& out) const;
    std::uint16_t len() const noexcept;
    void setOpCodeIdx(std::uint16_t idx) noexcept;
    std::uint16_t opCodeIdx() const noexcept;

    void pushByte(std::uint8_t byte);
    void pushTwoBytes(std::uint16_t bytes);

private:
    std::uint16_t opCodeIdx_;

    // byte structure
    const OPCode op_;
    std::vector<std::uint8_t> bytes_;
};

} // namespace instr