#ifndef INCLUDED_TIMERUTIL_H
#define INCLUDED_TIMERUTIL_H

#include "timer/timertypes.h"

#include <cstdint>

namespace TimerUtil
{

TimeMs64 toMs64(uint32_t nowMs32,
                uint32_t& high,
                uint32_t& last);

}

#endif
