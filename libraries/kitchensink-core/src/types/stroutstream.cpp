#include "types/stroutstream.h"

#include "types/strref.h"

#include <cstdio>
#include <cstring>

void StrOutStream::reset() const
{
    *mLength = 0;
}

const StrOutStream& StrOutStream::appendStr(const StrRef& str) const
{
    auto length(*mLength);
    auto copyLen(std::min(str.length(), mCapacity - length));
    
    std::copy(str.begin(),
              str.begin() + copyLen,
              mData + length);

    *mLength += copyLen;
    
    return *this;
}

const StrOutStream& StrOutStream::appendChar(char c) const
{
    if (*mLength < mCapacity)
    {
        mData[*mLength] = c;
        ++(*mLength);
    }
    
    return *this;
}

const StrOutStream& StrOutStream::appendInt(int n, const char* fmt) const
{
    char blk[64];

    std::snprintf(blk, sizeof(blk), fmt, n);

    appendStr(StrRef(blk));

    return *this;
}
