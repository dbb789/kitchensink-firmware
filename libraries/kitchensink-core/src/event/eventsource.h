#ifndef INCLUDED_EVENTSOURCE_H
#define INCLUDED_EVENTSOURCE_H

class EventStage;

class EventSource
{
public:
    virtual void pollEvent(EventStage& next) = 0;
    virtual bool hasPendingEvents() const;
};


inline
bool EventSource::hasPendingEvents() const
{
    return false;
}

#endif
