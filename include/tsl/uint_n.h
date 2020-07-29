/**
 * MIT License
 *
 * Copyright (c) 2020 Thibaut Goetghebuer-Planchon <tessil@gmx.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef TSL_UINT_N_H
#define TSL_UINT_N_H

#include <array>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits.h>
#include <ostream>
#include <type_traits>


namespace tsl {

#ifdef TSL_DEBUG
#    define tsl_uint_assert(expr) assert(expr)
#else
#    define tsl_uint_assert(expr) (static_cast<void>(0))
#endif

inline uint64_t builtin_popcount(uint64_t x) noexcept {
#if defined(__GNUG__) || defined(__clang__)
    return static_cast<uint64_t>(__builtin_popcountll(x));
#elif defined(_MSC_VER)
    return __popcnt64(x);
#else
#error compiler is not supported
#endif
}

/**
 * `BITS_COUNT` bit size unisgned integer
 */
template<std::size_t BITS_COUNT, class UINT_TYPE = uint64_t>
class uint_n {
    static_assert(BITS_COUNT > 0, "BITS_COUNT should be grater than 0");

public:
    using uint_type = UINT_TYPE;
    static constexpr auto N = BITS_COUNT;
    static constexpr auto S = sizeof(uint_type) * CHAR_BIT;
    using storage_type = std::array<uint_type, (N + S - 1) / S>;

    uint_n() = default;

    template<class... Args, class = typename std::enable_if<(sizeof...(Args) > 1) &&
                                                            (sizeof...(Args) == storage_type().size())>::type>
    uint_n(Args... args) noexcept: bits{{args...}} {
    }

    explicit uint_n(uint_type x) noexcept: bits() {
        bits.back() = x;
    }

    uint_n operator<<(size_t pos) const noexcept {
        return uint_n(*this) <<= pos;
    }

    uint_n& operator<<=(size_t pos) noexcept {
        for(size_t i = 0; i < bits.size(); ++i) {
            auto const upper = pos / S;
            auto const lower = upper + 1;
            auto const shift = pos % S;

            // upper
            if(i + upper < bits.size()) {
                bits[i] = bits[i + upper] << shift;
            } else {
                bits[i] = 0;
                continue;
            }

            // lower
            if(i + lower < bits.size()) {
                bits[i] |= bits[i + lower] >> (S - shift);
            }
        }
        return *this;
    }

    uint_n& operator&=(const uint_n& rhs) noexcept {
        for (size_t i = 0; i < bits.size(); ++i) {
            bits[i] &= rhs.bits[i];
        }
        return *this;
    }

    uint_n operator&(const uint_n& rhs) const noexcept {
        return uint_n(*this) &= rhs;
    }

    uint_n& operator--() noexcept {
        for(size_t i = bits.size() - 1; i != size_t(0) - 1; --i) {
            if (bits[i] != 0) {
                --bits[i];
                break;
            } else {
                --bits[i];
            }
        }
        return *this;
    }

    friend size_t popcount(const uint_n& x) noexcept {
        auto first = N % S ? N % S : S;
        size_t ret = (first == sizeof(uint_type) * CHAR_BIT)
            ? 0
            : builtin_popcount(((uint_type(1) << first) - 1) & x.bits[0]);
        for(size_t i = 1; i < x.bits.size(); ++i) {
            ret += builtin_popcount(x.bits[i]);
        }
        return ret;
    }

    bool test(size_t pos) const noexcept {
        tsl_uint_assert(pos < N);
        const auto local_pos = pos % S;
        const auto global_pos = pos / S;
        return bits[bits.size() - global_pos - 1] & (uint_type(1) << local_pos);
    }

    void set(size_t pos) noexcept {
        tsl_uint_assert(pos < N);
        const auto local_pos = pos % S;
        const auto global_pos = pos / S;
        bits[bits.size() - global_pos - 1] |= (uint_type(1) << local_pos);
    }

    void unset(size_t pos) noexcept {
        tsl_uint_assert(pos < N);
        const auto local_pos = pos % S;
        const auto global_pos = pos / S;
        bits[bits.size() - global_pos - 1] &= ~(uint_type(1) << local_pos);
    }

    friend std::ostream& operator<<(std::ostream& os, const uint_n& x) {
        constexpr auto first = N % S ? N % S : S;
        os << std::bitset<first>(x.bits.front());
        for(size_t i = 1; i < x.bits.size(); ++i) {
            os << "'" << std::bitset<S>(x.bits[i]);
        }
        return os;
    }

private:
    storage_type bits;
};

} // namespace tsl

#endif // TSL_UINT_N_H
