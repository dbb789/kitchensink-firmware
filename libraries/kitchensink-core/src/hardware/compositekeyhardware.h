#ifndef INCLUDED_COMPOSITEKEYHARDWARE_H
#define INCLUDED_COMPOSITEKEYHARDWARE_H

#include "hardware/keyhardware.h"
#include "hardware/keyhardwareeventhandler.h"

#include <array>
#include <cstdint>

template <std::size_t Size>
class CompositeKeyHardware : public KeyHardware
{
public:
    typedef std::array<KeyHardware*, Size> Entries;

public:
    CompositeKeyHardware(const Entries& entries);

public:
    virtual void poll(uint32_t                       timeMs,
                      const KeyHardwareEventHandler& handler) override;

    virtual void currentlyPressed(const KeyHardwareEventHandler& callback) override;
    
    virtual bool anyPressed() const override;

private:
    Entries mEntries;

private:
    CompositeKeyHardware(const CompositeKeyHardware&) = delete;
    CompositeKeyHardware& operator=(const CompositeKeyHardware&) = delete;
};


template <std::size_t Size>
inline
CompositeKeyHardware<Size>::CompositeKeyHardware(const Entries& entries)
    : mEntries(entries)
{ }

template <std::size_t Size>
inline
void CompositeKeyHardware<Size>::poll(uint32_t                       timeMs,
                                      const KeyHardwareEventHandler& handler)
{
    for (auto entry : mEntries)
    {
        entry->poll(timeMs, handler);
    }    
}

template <std::size_t Size>
inline
void CompositeKeyHardware<Size>::currentlyPressed(const KeyHardwareEventHandler& handler)
{
    for (auto entry : mEntries)
    {
        entry->currentlyPressed(handler);
    }    
}

template <std::size_t Size>
inline
bool CompositeKeyHardware<Size>::anyPressed() const
{
    bool anyPressed(false);

    for (auto entry : mEntries)
    {
        anyPressed |= entry->anyPressed();
    }    

    return anyPressed;
}


#endif
