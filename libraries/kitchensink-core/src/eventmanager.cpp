#include "eventmanager.h"

#include "event/eventsource.h"

EventManager::EventManager(EventSource&        eventSource,
                           EventStage&         rootEventStage,
                           ToplevelEventStage& toplevel,
                           EventStage&         nDefaultOutput)
    : defaultOutput(nDefaultOutput)
    , mEventSource(eventSource)
    , mEventBuffer()
    , mRootEventStage(rootEventStage)
    , mToplevel(toplevel)
{ }

bool EventManager::processEvent(const Event& event)
{
    // Buffer overflow is a no-op as it's generally never supposed to happen.
    if (!mEventBuffer.full())
    {
        mEventBuffer.pushBack(event);
    }
    
    return true;
}

void EventManager::poll(EventStage& output)
{
    ToplevelEventStage::OutputGuard guard(mToplevel, output);

    if (!mEventBuffer.empty())
    {
        mRootEventStage.processEvent(mEventBuffer.popFront());
    }
    
    mEventSource.pollEvent(mRootEventStage);
}

void EventManager::flush(EventStage& output)
{
    ToplevelEventStage::OutputGuard guard(mToplevel, output);
    
    do
    {
        mEventSource.pollEvent(mRootEventStage);
    }
    while (mEventSource.hasPendingEvents());
}
