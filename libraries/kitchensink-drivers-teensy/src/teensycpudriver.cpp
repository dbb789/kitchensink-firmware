#include "teensycpudriver.h"

#include <Arduino.h>

#ifdef TEENSYDUINO
#include <core_pins.h>
#endif

extern "C" char* sbrk(int incr);

void TeensyCpuDriver::bootloader()
{
    _reboot_Teensyduino_();
}

std::size_t TeensyCpuDriver::freeMemory()
{
    // http://forum.pjrc.com/threads/23256-Get-Free-Memory-for-Teensy-3-0

    char top;
    return &top - reinterpret_cast<char*>(sbrk(0));
}

uint32_t TeensyCpuDriver::nowMs()
{
    return millis();
}
