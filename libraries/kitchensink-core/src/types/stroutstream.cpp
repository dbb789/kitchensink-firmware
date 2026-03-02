#include "types/stroutstream.h"

#include "types/strref.h"

#include <cstdio>
#include <cstring>

void StrOutStream::reset() const
{
    mData[0] = '\0';
}

const StrOutStream& StrOutStream::appendStr(const StrRef& str) const
{
    auto currentLength(strlen(mData));

    snprintf(mData + currentLength,
             mDataSize - currentLength,
             "%.*s", static_cast<int>(str.length()), str.begin());
    
    return *this;
}

const StrOutStream& StrOutStream::appendChar(char c) const
{
    auto currentLength(strlen(mData));

    if (currentLength < mDataSize - 1)
    {
        mData[currentLength] = c;
        mData[currentLength + 1] = '\0';
    }

    return *this;
}

const StrOutStream& StrOutStream::appendInt(int n, const char* fmt) const
{
    auto currentLength(strlen(mData));

    snprintf(mData + currentLength, mDataSize - currentLength, fmt, n);

    return *this;
}
