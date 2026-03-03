#ifndef INCLUDED_KEYBOARDPLATE_H
#define INCLUDED_KEYBOARDPLATE_H

#include "hardware/debounce.h"
#include "hardware/keyhardware.h"
#include "hardware/keyhardwareevent.h"
#include "hardware/keyhardwareeventhandler.h"
#include "types/bitmask2d.h"

#include <cstdint>

template <typename KeyDriver, std::size_t Columns, std::size_t Rows>
class KeyboardPlate : public KeyHardware
{
private:
    typedef Bitmask2d<Columns, Rows> KeyMask;

public:
    KeyboardPlate(KeyDriver&                          keyDriver,
                  const std::array<uint8_t, Columns>& columnMapping,
                  const std::array<uint8_t, Rows>&    rowMapping);

public:
    virtual void poll(uint32_t                       timeMs,
                      const KeyHardwareEventHandler& eventHandler) override;

    virtual void currentlyPressed(const KeyHardwareEventHandler& eventHandler) override;
    
    virtual bool anyPressed() const override;

private:
    void dispatch(const KeyMask&                 stateMask,
                  const KeyMask&                 deltaMask,
                  const KeyHardwareEventHandler& eventHandler) const;

private:
    KeyMask                                mState;
    KeyDriver&                             mKeyDriver;
    Debounce<KeyMask>                      mDebounce;
    std::array<uint8_t, KeyMask::kColumns> mColumnMapping;
    std::array<uint8_t, KeyMask::kRows>    mRowMapping;

private:
    KeyboardPlate(const KeyboardPlate&) = delete;
    KeyboardPlate& operator=(const KeyboardPlate&) = delete;
};


template <typename KeyDriver, std::size_t Columns, std::size_t Rows>
inline
KeyboardPlate<KeyDriver, Columns, Rows>::KeyboardPlate(KeyDriver&                          keyDriver,
                                                       const std::array<uint8_t, Columns>& columnMapping,
                                                       const std::array<uint8_t, Rows>&    rowMapping)
    : mState()
    , mKeyDriver(keyDriver)
    , mDebounce()
    , mColumnMapping(columnMapping)
    , mRowMapping(rowMapping)
{ }

template <typename KeyDriver, std::size_t Columns, std::size_t Rows>
inline
void KeyboardPlate<KeyDriver, Columns, Rows>::poll(uint32_t                       timeMs,
                                                   const KeyHardwareEventHandler& eventHandler)
{
    mKeyDriver.scan(mState);
    
    if (mDebounce.process(timeMs, mState))
    {
        dispatch(mDebounce.state,
                 mDebounce.delta,
                 eventHandler);
    }
}

template <typename KeyDriver, std::size_t Columns, std::size_t Rows>
inline
void KeyboardPlate<KeyDriver, Columns, Rows>::currentlyPressed(const KeyHardwareEventHandler& eventHandler)
{
    dispatch(KeyMask(),
             mDebounce.state,
             eventHandler);
}

template <typename KeyDriver, std::size_t Columns, std::size_t Rows>
inline
bool KeyboardPlate<KeyDriver, Columns, Rows>::anyPressed() const
{
    return !mDebounce.state.empty() || !mDebounce.delta.empty();
}

template <typename KeyDriver, std::size_t Columns, std::size_t Rows>
inline
void KeyboardPlate<KeyDriver, Columns, Rows>::dispatch(const KeyMask&                 stateMask,
                                                       const KeyMask&                 deltaMask,
                                                       const KeyHardwareEventHandler& eventHandler) const
{
    for (std::size_t row(0); row < KeyMask::kRows; ++row)
    {
        const auto& state(stateMask[row]);
        const auto& delta(deltaMask[row]);

        auto deltaIterator(delta.bitIterator());
        
        while (deltaIterator.more())
        {
            auto column(deltaIterator.next());

            if (column < KeyMask::kColumns)
            {
                eventHandler.processKeyHardwareEvent(KeyHardwareEvent(mRowMapping[row],
                                                                      mColumnMapping[column],
                                                                      state[column]));
            }
        }
    }
}

#endif
