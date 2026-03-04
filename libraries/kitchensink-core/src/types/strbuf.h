#ifndef INCLUDED_STRBUF_H
#define INCLUDED_STRBUF_H

#include "types/strref.h"

#include <array>
#include <cstdint>

template <std::size_t Capacity>
class StrBuf
{
private:
    typedef std::array<char, Capacity> Data;
    
public:
    typedef typename Data::const_iterator const_iterator;
    typedef typename Data::iterator       iterator;

public:
    StrBuf();
    StrBuf(const StrRef& strRef);

public:
    StrBuf<Capacity>& insert(iterator it, char c);
    StrBuf<Capacity>& erase(iterator it);
    
public:
    constexpr const_iterator begin() const;
    constexpr const_iterator end() const;

    iterator begin();
    iterator end();
    
    constexpr std::size_t capacity() const;
    constexpr std::size_t length() const;
    constexpr bool empty() const;

    void clear();
    void clearSecure();
    
public:
    StrBuf<Capacity>& operator=(const StrRef& strRef);
    constexpr operator StrRef() const;

    constexpr const char& operator[](std::size_t n) const;
    char& operator[](std::size_t n);
    
private:
    std::size_t mLength;
    Data        mData;

private:
    friend class StrOutStream;
};


template <std::size_t Capacity>
inline
StrBuf<Capacity>::StrBuf()
    : mLength(0)
{
}

template <std::size_t Capacity>
inline
StrBuf<Capacity>::StrBuf(const StrRef& strRef)
{
    *this = strRef;
}

template <std::size_t Capacity>
inline
StrBuf<Capacity>& StrBuf<Capacity>::insert(iterator it, char c)
{
    // Inserting into a full buffer is an error by the caller which we'll
    // dismiss as a no-op and ignore.
    if (length() >= capacity())
    {
        return *this;
    }
    
    auto endIt(end());
    
    std::move_backward(it, endIt, endIt + 1);

    *it = c;

    ++mLength;
    
    return *this;
}

template <std::size_t Capacity>
inline
StrBuf<Capacity>& StrBuf<Capacity>::erase(iterator it)
{
    std::move(it + 1, end(), it);

    --mLength;
    
    return *this;
}

template <std::size_t Capacity>
inline
void StrBuf<Capacity>::clear()
{
    mLength = 0;
}

template <std::size_t Capacity>
inline
void StrBuf<Capacity>::clearSecure()
{
    clear();

    // Will not be optimised away as described here:
    // https://en.cppreference.com/w/cpp/string/byte/memset
    // Also note that we're clearing the full buffer here.
    std::fill(reinterpret_cast<volatile char*>(begin()),
              reinterpret_cast<volatile char*>(begin()) + sizeof(mData),
              '\0');
}

template <std::size_t Capacity>
inline
constexpr typename StrBuf<Capacity>::const_iterator StrBuf<Capacity>::begin() const
{
    return mData.begin();
}

template <std::size_t Capacity>
inline
constexpr typename StrBuf<Capacity>::const_iterator StrBuf<Capacity>::end() const
{
    return mData.begin() + length();
}

template <std::size_t Capacity>
inline
typename StrBuf<Capacity>::iterator StrBuf<Capacity>::begin()
{
    return mData.begin();
}

template <std::size_t Capacity>
inline
typename StrBuf<Capacity>::iterator StrBuf<Capacity>::end()
{
    return mData.begin() + length();
}

template <std::size_t Capacity>
inline
constexpr std::size_t StrBuf<Capacity>::length() const
{
    return mLength;
}

template <std::size_t Capacity>
inline
constexpr bool StrBuf<Capacity>::empty() const
{
    return mLength == 0;
}

template <std::size_t Capacity>
inline
constexpr std::size_t StrBuf<Capacity>::capacity() const
{
    return mData.size();
}

template <std::size_t Capacity>
inline
StrBuf<Capacity>& StrBuf<Capacity>::operator=(const StrRef& strRef)
{
    mLength = std::min(strRef.length(), capacity());
    
    std::copy(strRef.begin(),
              strRef.begin() + mLength,
              mData.begin());
    
    return *this;
}

template <std::size_t Capacity>
inline
constexpr StrBuf<Capacity>::operator StrRef() const
{
    return StrRef(begin(), end());
}

template <std::size_t Capacity>
inline
constexpr const char& StrBuf<Capacity>::operator[](std::size_t n) const
{
    return mData[n];
}

template <std::size_t Capacity>
inline
char& StrBuf<Capacity>::operator[](std::size_t n)
{
    return mData[n];
}

#endif
