#include "event/delayevent.h"

bool DelayEvent::createEvents(uint32_t& delayMs, Event& event)
{
    if (delayMs == 0)
    {
        return false;
    }
    
    if (delayMs > kMaxDelayMs)
    {
        event = create(kMaxDelayMs);
        delayMs -= kMaxDelayMs;
    }
    else
    {
        event = create(delayMs);
        delayMs = 0;
    }
    
    return true;
}

Event DelayEvent::create(uint32_t delayMs)
{
    return Event(kType, delayMs >> 8, delayMs);
}

