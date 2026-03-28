#include "ui/uitimers.h"

UITimers::UITimers(TimerManager& timerManager)
    : mFlashTimer(timerManager.createTimer())
    , mCursorTimer(timerManager.createTimer())
{
    schedule();
}

void UITimers::reset()
{
    schedule();
}

void UITimers::schedule()
{
    mFlashTimer.scheduleRepeating(250, 250);
    mCursorTimer.scheduleRepeating(1000, 500);
}
