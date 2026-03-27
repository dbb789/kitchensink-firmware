#include "testutil/testutil.h"
#include <exception>

uint8_t TestUtil::hexNibble(char c)
{
    if (c >= '0' && c <= '9')
    {
        return uint8_t(c - '0');
    }
    else if (c >= 'a' && c <= 'f')
    {
        return uint8_t(c - 'a' + 10);
    }
    else if (c >= 'A' && c <= 'F')
    {
        return uint8_t(c - 'A' + 10);
    }

    return 0;
}

uint8_t TestUtil::hexByte(char high, char low)
{
    return uint8_t((hexNibble(high) << 4) | hexNibble(low));
}

void TestUtil::hexToVector(const StrRef&         hex,
                           std::vector<uint8_t>& out)
{
    for (std::size_t i(0); i + 1 < hex.length(); i += 2)
    {
        out.push_back(hexByte(hex[i], hex[i + 1]));
    }
}
