#ifndef INCLUDED_EVENTMANAGER_H
#define INCLUDED_EVENTMANAGER_H

#include "types/circularbuffer.h"
#include "event/eventstage.h"
#include "event/event.h"
#include "topleveleventstage.h"

#include <cstdint>

class EventSource;

class EventManager : public EventStage
{
public:
    EventManager(EventSource&        eventSource,
                 EventStage&         rootEventStage,
                 ToplevelEventStage& toplevel,
                 EventStage&         nDefaultOutput);

public:
    virtual bool processEvent(const Event& event) override;

    void poll(EventStage& output);
    void flush(EventStage& output);
    
public:
    EventStage& defaultOutput;
    
private:
    EventSource&              mEventSource;
    CircularBuffer<Event, 48> mEventBuffer;
    EventStage&               mRootEventStage;
    ToplevelEventStage&       mToplevel;

private:
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;
};

#endif








