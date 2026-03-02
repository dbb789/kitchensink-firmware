#ifndef INCLUDED_MULTIKEYPROCESSOR_H
#define INCLUDED_MULTIKEYPROCESSOR_H

#include "multikeyset.h"
#include "event/eventstage.h"
#include "timer/timermanager.h"

#include <cstdint>

class Event;
class GlobalConfig;

class MultiKeyProcessor : public EventStage
{
public:
    MultiKeyProcessor(MultiKeySet&  multiSet,
                      GlobalConfig& globalConfig,
                      TimerManager& timer,
                      EventStage&   next);

public:
    virtual bool processEvent(const Event& event) override;

private:
    MultiKeySet&  mMultiKeySet;
    GlobalConfig& mGlobalConfig;
    Timer         mReleaseTimer;
    int           mLast;
    EventStage&   mNext;
};

inline
MultiKeyProcessor::MultiKeyProcessor(MultiKeySet&  multiSet,
                                     GlobalConfig& globalConfig,
                                     TimerManager& timer,
                                     EventStage&   next)
    : mMultiKeySet(multiSet)
    , mGlobalConfig(globalConfig)
    , mReleaseTimer(timer.createTimer())
    , mLast(-1)
    , mNext(next)
{ }

#endif
