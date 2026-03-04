#ifndef INCLUDED_ARDUINOCPUDRIVER_H
#define INCLUDED_ARDUINOCPUDRIVER_H

#include "hardware/cpuutil.h"

class ArduinoCpuDriver : public CpuUtil::Driver
{
public:
    ArduinoCpuDriver() = default;

public:
    virtual void bootloader() override;
    virtual std::size_t freeMemory() override;
    virtual uint32_t nowMs() override;    
};

#endif
