#pragma once

#include <cstdint>
#include <ostream>

namespace instr {

enum class OPCode : std::uint8_t {
    // ... 
};

class IInstr {
public:
    IInstr(OPCode op);

    virtual ~IInstr() = default;

public:
    virtual std::uint32_t len() const noexcept = 0;
    virtual void printBytes(std::ostream& out) const = 0;

public:
    std::uint32_t setOpCodeIdx() noexcept;
    std::uint32_t opCodeIdx() const noexcept;
    
private:
    const OPCode op_;
    std::uint32_t opCodeIdx_;
};

} // namespace instr