#ifndef INCLUDED_TESTUTIL_H
#define INCLUDED_TESTUTIL_H

#include "gtest/gtest.h"

#include "types/strref.h"

#include <cstdint>
#include <vector>

namespace TestUtil
{

uint8_t hexNibble(char c);

uint8_t hexByte(char high, char low);

void hexToVector(const StrRef&         hex,
                 std::vector<uint8_t>& out);

template <std::size_t Capacity>
inline
void hexToArray(const StrRef&         hex,
                std::array<uint8_t, Capacity>& out)
{
    ASSERT_EQ(hex.length(), Capacity * 2);
    
    for (std::size_t i(0); i < Capacity; ++i)
    {
        out[i] = hexByte(hex[i * 2], hex[(i * 2) + 1]);
    }
}

};

#endif
