#include "layerprocessor.h"

#include "event/event.h"
#include "keysource.h"
#include "event/layerevent.h"

LayerProcessor::LayerProcessor(EventStage& next)
    : mKeySource(nullptr)
    , mNext(next)
{ }

void LayerProcessor::setKeySource(KeySource* keySource)
{
    mKeySource = keySource;
}

bool LayerProcessor::processEvent(const Event& event)
{
    if (event.is<LayerEvent>())
    {
        if (mKeySource)
        {
            auto layerEvent(event.get<LayerEvent>());
            
            mKeySource->setLayer(layerEvent.layer,
                                 layerEvent.enable);
        }
    }
    else
    {
        mNext.processEvent(event);
    }

    return true;
}
