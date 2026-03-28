#include "timer/timerutil.h"

TimeMs64 TimerUtil::toMs64(uint32_t nowMs32,
                           uint32_t& high,
                           uint32_t& last)
{
    if (nowMs32 < last)
    {
        ++high;
    }

    last = nowMs32;

    return (static_cast<TimeMs64>(high) << 32) | nowMs32;
}
