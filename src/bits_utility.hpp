#include <ostream>

#include <concepts>

namespace utility {

// template <class Numeric>
inline void printBytes(std::ostream& out, std::integral v) {
    out.write(
        reinterpret_cast<char*>(&v),
        sizeof(v)
    );
}

template <class Integral>
    requires std::integral<Integral>
inline Integral reverse(Integral x) {
    Integral res = 0;
    Integral mask = 0xff;
    for (int i = 0; i < 4; ++i) {
        res <<= 8;
        res |= x & mask;
        x >>= 8;
    }
    return res;
}

} // namespace utility