#include "arduinocpudriver.h"

#include <Arduino.h>

extern "C" char* sbrk(int incr);

void ArduinoCpuDriver::bootloader()
{
    // Not implemented.
}

std::size_t ArduinoCpuDriver::freeMemory()
{
    // http://forum.pjrc.com/threads/23256-Get-Free-Memory-for-Arduino-3-0

    char top;
    return &top - reinterpret_cast<char*>(sbrk(0));
}

uint32_t ArduinoCpuDriver::nowMs()
{
    return millis();
}
