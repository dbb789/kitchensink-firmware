#include "multikeyprocessor.h"

#include "event/event.h"
#include "event/multievent.h"
#include "globalconfig.h"

bool MultiKeyProcessor::processEvent(const Event& event)
{
    if (mReleaseTimer.matches(event))
    {
        if (mLast >= 0)
        {
            mMultiKeySet[mLast].trigger(mNext);
        }
    }
    else if (event.is<MultiEvent>())
    {
        auto multiEvent(event.get<MultiEvent>());
        auto multiId(multiEvent.multiId);
        
        if (multiId < mMultiKeySet.size())
        {
            if (multiEvent.pressed)
            {
                mReleaseTimer.schedule(mGlobalConfig.tapDelay);
                mMultiKeySet[multiId].press();
            }
            else
            {
                mMultiKeySet[multiId].release(mNext);
            }
            
            if (multiId != mLast && mLast >= 0)
            {
                mMultiKeySet[mLast].trigger(mNext);
            }
            
            mLast = static_cast<int>(multiId);
        }
    }
    else
    {
        if (mLast >= 0)
        {
            mMultiKeySet[mLast].trigger(mNext);
        }
        
        mNext.processEvent(event);   
    }

    return true;
}
