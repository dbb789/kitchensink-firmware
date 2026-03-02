#ifndef INCLUDED_LAYERPROCESSOR_H
#define INCLUDED_LAYERPROCESSOR_H

#include "event/eventstage.h"

class KeySource;
class Event;

class Event;

class LayerProcessor : public EventStage
{
public:
    explicit LayerProcessor(EventStage& next);

public:
    void setKeySource(KeySource* keySource);
    virtual bool processEvent(const Event& event) override;
    
private:
    KeySource* mKeySource;
    EventStage& mNext;
    
private:
    LayerProcessor(const LayerProcessor&) = delete;
    LayerProcessor& operator=(const LayerProcessor&) = delete;
};

#endif
