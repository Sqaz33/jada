#pragma once

#include <cstdint>

namespace codegen {

inline namespace java_bytecode_codegen  {

enum class FundamentalType {
    INT,
    FLOAT,
    DOUBLE,
    LONG,
    BOOLEAN
};

enum class AccessFlag : std::uint16_t {
    ACC_PUBLIC    = 0x0001,
    ACC_PRIVATE   = 0x0002,
    ACC_PROTECTED = 0x0004,
    ACC_STATIC    = 0x0008,
    ACC_FINAL     = 0x0010,
    ACC_VOLATILE  = 0x0040,
    ACC_TRANSIENT = 0x0080,
    ACC_SYNTHETIC = 0x1000,
    ACC_ENUM      = 0x4000
};

} // namespace java_bytecode_codegen  

} // namespace codegen