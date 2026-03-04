#ifndef INCLUDED_STROUTSTREAM_H
#define INCLUDED_STROUTSTREAM_H

#include "types/strref.h"
#include "types/strbuf.h"
#include "types/stroutstream.h"
#include "types/outstream.h"
#include "types/dataref.h"

#include <cstdint>
#include <cstring>

class StrOutStream
{
public:
    template <std::size_t Capacity>
    StrOutStream(StrBuf<Capacity>& buf);

public:
    StrRef str() const;
    void reset() const;

public:
    const StrOutStream& appendStr(const StrRef& str) const;
    const StrOutStream& appendChar(char c) const;
    const StrOutStream& appendInt(int n, const char* fmt = "%d") const;
    
private:
    std::size_t  mCapacity;
    std::size_t* mLength;
    char*        mData;

private:
    StrOutStream(const StrOutStream&) = delete;
    StrOutStream& operator=(const StrOutStream&) = delete;
};


template <std::size_t Capacity>
inline
StrOutStream::StrOutStream(StrBuf<Capacity>& buf)
    : mCapacity(buf.capacity())
    , mLength(&buf.mLength)
    , mData(buf.begin())
{ }

inline
StrRef StrOutStream::str() const
{
    return StrRef(mData, mData + *mLength);
}

#endif




