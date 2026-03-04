#ifndef INCLUDED_CPUUTIL_H
#define INCLUDED_CPUUTIL_H

#include <cstdint>

class CpuUtil
{
public:
    class Driver
    {
    public:
        Driver() = default;
        virtual ~Driver() = default;
        
    public:
        virtual void bootloader() = 0;
        virtual std::size_t freeMemory() = 0;
        virtual uint32_t nowMs() = 0;
        
    private:
        Driver(const Driver&) = delete;
        Driver& operator=(const Driver&) = delete;
    };
    
private:
    static Driver* mDriver;
    
public:
    static void setDriver(Driver* driver);
    static void bootloader();
    static std::size_t freeMemory();
    static uint32_t nowMs();
};

#endif
