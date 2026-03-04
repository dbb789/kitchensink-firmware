#include "eventrecorder.h"

#include "event/delayevent.h"
#include "event/screenevent.h"
#include "event/tickevent.h"
#include "hardware/ctrlutil.h"

EventRecorder::EventRecorder(bool realtime)
    : mLastMs(0)
    , mRealtime(realtime)
    , mContent()
    , mSize(0)
    , mComplete(false)
{ }

bool EventRecorder::processEvent(const Event& event)
{
    if (event != ScreenEvent::create(ScreenEvent::Type::kHome))
    {
        if (event.isUserEvent())
        {
            if (mRealtime)
            {
                auto nowMs(CtrlUtil::nowMs());
                
                if (mLastMs != 0)
                {
                    Event event;
                    uint32_t durationMs(nowMs - mLastMs);
                    
                    while (DelayEvent::createEvents(durationMs, event)
                           && mSize < mContent.size())
                    {
                        mContent[mSize++] = event;
                    }
                }
                
                mLastMs = nowMs;
            }
            
            if (mSize < mContent.size())
            {
                mContent[mSize++] = event;
            }
        }
    }
    else
    {
        mComplete = true;
    }

    return true;
}







