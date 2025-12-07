#pragma once

#include <ostream>

#include <concepts>

namespace utility {

template <std::integral Integral>
void printBytes(std::ostream& out, Integral v) {
    out.write(
        reinterpret_cast<char*>(&v),
        sizeof(Integral)
    );
}

template <std::integral Integral>
Integral reverse(Integral x) {
    Integral res = 0;
    Integral mask = 0xff;
    for (std::uint64_t i = 0; i < sizeof(Integral); ++i) {
        res <<= 8;
        res |= x & mask;
        x >>= 8;
    }
    return res;
}

} // namespace utility