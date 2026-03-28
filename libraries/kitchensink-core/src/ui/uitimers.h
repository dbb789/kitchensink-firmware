#ifndef INCLUDED_UITIMERS_H
#define INCLUDED_UITIMERS_H

#include "timer/timermanager.h"

class UITimers
{
public:
    explicit UITimers(TimerManager& timerManager);

public:
    const Timer& flashTimer();
    const Timer& cursorTimer();

public:
    void reset();
    
private:
    void schedule();
    
private:
    Timer mFlashTimer;
    Timer mCursorTimer;
    
private:
    UITimers(const UITimers&) = delete;
    UITimers& operator=(const UITimers&) = delete;
};


inline
const Timer& UITimers::flashTimer()
{
    return mFlashTimer;
}

inline
const Timer& UITimers::cursorTimer()
{
    return mCursorTimer;
}

#endif
    
