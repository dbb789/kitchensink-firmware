#ifndef INCLUDED_DELAYEVENT_H
#define INCLUDED_DELAYEVENT_H

#include "event/event.h"

class DelayEvent
{
public:
    static constexpr Event::Type kType = Event::Type::kDelay;
    static constexpr uint32_t kMaxDelayMs = 0xfff;
    
public:
    static bool createEvents(uint32_t& delayMs, Event& event);
    static Event create(uint32_t delayMs);
    
private:
    explicit constexpr DelayEvent(const Event& event);
    
public:
    uint32_t delayMs;
    
private:
    friend class Event;
};


inline
constexpr DelayEvent::DelayEvent(const Event& event)
     : delayMs((event.subType() << 8) | event.value())
{ }

#endif
