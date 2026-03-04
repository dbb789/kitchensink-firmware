#include "hardware/cpuutil.h"

CpuUtil::Driver* CpuUtil::mDriver = nullptr;

void CpuUtil::setDriver(Driver* driver)
{
    mDriver = driver;
}

void CpuUtil::bootloader()
{
    if (mDriver)
    {
        mDriver->bootloader();
    }
}

std::size_t CpuUtil::freeMemory()
{
    if (mDriver)
    {
        return mDriver->freeMemory();
    }

    return 0;
}

uint32_t CpuUtil::nowMs()
{
    if (mDriver)
    {
        return mDriver->nowMs();
    }

    return 0;
}
