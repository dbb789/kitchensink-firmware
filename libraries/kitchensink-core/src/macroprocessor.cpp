#include "macroprocessor.h"
#include "types/range.h"
#include "event/event.h"
#include "event/macroevent.h"
#include "event/delayevent.h"
#include "event/screenevent.h"
#include "macro.h"
#include "macroset.h"
#include "globalconfig.h"

MacroProcessor::MacroProcessor(const MacroSet&       macroSet,
                               const SecureMacroSet& secureMacroSet,
                               const GlobalConfig&   globalConfig,
                               TimerManager&         timer,
                               EventStage&           next)
    : mMacroSet(macroSet)
    , mSecureMacroSet(secureMacroSet)
    , mGlobalConfig(globalConfig)
    , mCurrent(nullptr)
    , mCurrentState()
    , mCurrentCount(0)
    , mPlaybackTimer(timer.createTimer())
    , mNext(next)
{ }

bool MacroProcessor::processEvent(const Event& event)
{
    if (mPlaybackTimer.matches(event))
    {
        playback();
    }
    else if (event.is<MacroEvent>())
    {
        auto macroEvent(event.get<MacroEvent>());
        auto macroId(macroEvent.macroId);
        
        switch (macroEvent.type)
        {
        case MacroEvent::Type::kDefault:
            if (macroId < mMacroSet.size())
            {
                processMacro(mMacroSet[macroId],
                             macroEvent.pressed);
            }
            break;

        case MacroEvent::Type::kSecure:
            if (mSecureMacroSet.unlocked())
            {
                if (macroId < mSecureMacroSet.size())
                {
                    processMacro(mSecureMacroSet[macroId],
                                 macroEvent.pressed);
                }
            }
            else if (macroEvent.pressed)
            {
                mNext.processEvent(ScreenEvent::create(ScreenEvent::Type::kScreen,
                                                       ScreenEvent::kMacroUnlock));
            }
            break;
        }
    }
    else
    {
        mNext.processEvent(event);
    }

    return true;
}

void MacroProcessor::processMacro(const Macro& macro,
                                  bool         pressed)
{
    const auto& content(macro.content);
    const auto size(content.end() - content.begin());    

    // Exit early for no content playback.
    if (size == 0)
    {
        return;
    }
    
    if (!pressed && macro.type == Macro::Type::kInvert)
    {
        mCurrent = &macro;
        mCurrentState = content.end() - 1;
        mCurrentCount = -static_cast<int>(size);
                    
        playback();
    }
    else
    {
        if (pressed)
        {
            mCurrent = &macro;
            mCurrentState = content.begin();
            mCurrentCount = static_cast<int>(size);
                    
            playback();
        }
    }
}

void MacroProcessor::playback()
{
    if (mCurrent)
    {
        while (mCurrentCount != 0)
        {
            const auto& event(*mCurrentState);

            bool forward(mCurrentCount > 0);
            
            if (forward)
            {
                --mCurrentCount;

                // Avoid incrementing mCurrentState past bounds - technically UB
                // on the decrement case, but we'll do it both sides for
                // consistency.
                if (mCurrentCount != 0)
                {
                    ++mCurrentState;
                }
            }
            else
            {
                ++mCurrentCount;

                if (mCurrentCount != 0)
                {
                    --mCurrentState;
                }
            }
            
            if (event.is<DelayEvent>())
            {
                mPlaybackTimer.schedule(event.get<DelayEvent>().delayMs);
                return;
            }
            else
            {   
                mNext.processEvent(forward ? event : event.invert());

                auto macroPlaybackDelay(mGlobalConfig.macroPlaybackDelay);
                
                if (mCurrent->type != Macro::Type::kRealtime && macroPlaybackDelay > 0)
                {
                    mPlaybackTimer.schedule(macroPlaybackDelay);
                    return;
                }
            }
        }
        
        mCurrent = nullptr;
    }
}






