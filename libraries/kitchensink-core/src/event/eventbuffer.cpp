#include "event/eventbuffer.h"

void EventBuffer::pollEvent(EventStage& next)
{
    if (!mBuffer.empty())
    {
        next.processEvent(mBuffer.popFront());
    }
}

bool EventBuffer::processEvent(const Event& event)
{
    if (!mBuffer.full())
    {
        mBuffer.pushBack(event);
    }
    
    return true;
}
