#ifndef INCLUDED_TEENSYCPUDRIVER_H
#define INCLUDED_TEENSYCPUDRIVER_H

#include "hardware/cpuutil.h"

class TeensyCpuDriver : public CpuUtil::Driver
{
public:
    TeensyCpuDriver() = default;

public:
    virtual void bootloader() override;
    virtual std::size_t freeMemory() override;
    virtual uint32_t nowMs() override;    
};

#endif
