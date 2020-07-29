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
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "utils.h"
#include "tsl/uint_n.h"

template <size_t BITS_COUNT>
using uint_n = tsl::uint_n<BITS_COUNT, uint8_t>;

BOOST_AUTO_TEST_SUITE(test_bitset)

namespace {

template <size_t N>
std::string to_string(uint_n<N> const& x) {
    std::ostringstream oss;
    oss << x;
    return oss.str();
}

} // namespace

BOOST_AUTO_TEST_CASE(test_constructor_from_int) {
    uint_n<2> x(1);
    BOOST_CHECK_EQUAL(to_string(x), "01");
}

BOOST_AUTO_TEST_CASE(test_constructor_from_multiple_int) {
    uint_n<9> x(1_b, 101_b);
    BOOST_CHECK_EQUAL(to_string(x), "1'00000101");
}

BOOST_AUTO_TEST_CASE(test_shift_left_operator) {
    {
        uint_n<1> x(1);
        x <<= 1;
        BOOST_CHECK_EQUAL(to_string(x), "0");
    }

    {
        uint_n<2> x(01_b);
        x <<= 1;
        BOOST_CHECK_EQUAL(to_string(x), "10");
    }

    {
        uint_n<10> x(00000000_b, 10101001_b);
        x <<= 3;
        BOOST_CHECK_EQUAL(to_string(x), "01'01001000");
    }

    {
        uint_n<10> x(00000000_b, 10101001_b);
        x <<= 3;
        BOOST_CHECK_EQUAL(to_string(x), "01'01001000");
    }

    {
        uint_n<32> x(00000000_b, 10101001_b, 00001000_b, 00000100_b);
        x <<= 9;
        BOOST_CHECK_EQUAL(to_string(x), "01010010'00010000'00001000'00000000");
    }
}

BOOST_AUTO_TEST_CASE(test_operator_pre_decrement) {
    {
        uint_n<1> x(1_b);
        --x;
        BOOST_CHECK_EQUAL(to_string(x), "0");
        --x;
        BOOST_CHECK_EQUAL(to_string(x), "1");
    }

    {
        uint_n<10> x(1_b, 1_b);
        --x;
        BOOST_CHECK_EQUAL(to_string(x), "01'00000000");
        --x;
        BOOST_CHECK_EQUAL(to_string(x), "00'11111111");
    }

    {
        uint_n<9> x(0_b, 0_b);
        --x;
        BOOST_CHECK_EQUAL(to_string(x), "1'11111111");
    }
}

BOOST_AUTO_TEST_CASE(test_popcount) {
    BOOST_CHECK_EQUAL(popcount(uint_n<1>(0_b)), 0);
    BOOST_CHECK_EQUAL(popcount(uint_n<1>(1_b)), 1);
    BOOST_CHECK_EQUAL(popcount(uint_n<2>(11_b)), 2);
    BOOST_CHECK_EQUAL(popcount(uint_n<2>(111_b)), 2);
    BOOST_CHECK_EQUAL(popcount(uint_n<9>(1_b, 1_b)), 2);
}

BOOST_AUTO_TEST_CASE(test_operator_bitwise_and) {
    {
        uint_n<1> x(1_b);
        const uint_n<1> y(1_b);
        BOOST_CHECK_EQUAL(to_string(x &= y), "1");
    }

    {
        uint_n<1> x(0_b);
        const uint_n<1> y(1_b);
        BOOST_CHECK_EQUAL(to_string(x &= y), "0");
    }

    {
        uint_n<10> x(10_b, 111_b);
        const uint_n<10> y(11_b, 101_b);
        BOOST_CHECK_EQUAL(to_string(x &= y), "10'00000101");
    }

}

BOOST_AUTO_TEST_CASE(test_test) {
    {
        const uint_n<1> x(1_b);
        BOOST_CHECK(x.test(0));
    }

    {
        const uint_n<1> x(0_b);
        BOOST_CHECK(!x.test(0));
    }

    {
        const uint_n<11> x(101_b, 10010000_b);
        BOOST_CHECK(x.test(10));
        BOOST_CHECK(!x.test(9));
        BOOST_CHECK(x.test(8));
        BOOST_CHECK(x.test(7));
        BOOST_CHECK(!x.test(6));
        BOOST_CHECK(x.test(4));
        BOOST_CHECK(!x.test(3));
    }
}

BOOST_AUTO_TEST_CASE(test_set) {
    uint_n<11> x{};
    x.set(1);
    x.set(3);
    x.set(7);
    x.set(10);
    x.set(10);
    BOOST_CHECK_EQUAL(to_string(x), "100'10001010");
}

BOOST_AUTO_TEST_CASE(test_unset) {
    uint_n<11> x(101_b, 10001010_b);
    x.unset(0);
    x.unset(1);
    x.unset(8);
    BOOST_CHECK_EQUAL(to_string(x), "100'10001000");
}

BOOST_AUTO_TEST_SUITE_END()
