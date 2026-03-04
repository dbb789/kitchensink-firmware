#include "gtest/gtest.h"

#include "hardware/cpuutil.h"

namespace {

struct MockCpuDriver : public CpuUtil::Driver
{
    bool        bootloaderCalled  = false;
    std::size_t freeMemoryResult  = 0;
    uint32_t    nowMsResult       = 0;

    void bootloader() override
    {
        bootloaderCalled = true;
    }

    std::size_t freeMemory() override
    {
        return freeMemoryResult;
    }

    uint32_t nowMs() override
    {
        return nowMsResult;
    }
};

} // namespace

TEST(CpuUtil, NullDriverNowMsReturnsZero)
{
    CpuUtil::setDriver(nullptr);

    ASSERT_EQ(CpuUtil::nowMs(), 0);
}

TEST(CpuUtil, NullDriverFreeMemoryReturnsZero)
{
    CpuUtil::setDriver(nullptr);

    ASSERT_EQ(CpuUtil::freeMemory(), 0);
}

TEST(CpuUtil, NullDriverBootloaderDoesNothing)
{
    CpuUtil::setDriver(nullptr);

    CpuUtil::bootloader();
}

TEST(CpuUtil, SetDriverNowMs)
{
    MockCpuDriver driver;
    driver.nowMsResult = 42;

    CpuUtil::setDriver(&driver);

    ASSERT_EQ(CpuUtil::nowMs(), 42);

    CpuUtil::setDriver(nullptr);
}

TEST(CpuUtil, SetDriverFreeMemory)
{
    MockCpuDriver driver;
    driver.freeMemoryResult = 1024;

    CpuUtil::setDriver(&driver);

    ASSERT_EQ(CpuUtil::freeMemory(), 1024);

    CpuUtil::setDriver(nullptr);
}

TEST(CpuUtil, SetDriverBootloader)
{
    MockCpuDriver driver;

    CpuUtil::setDriver(&driver);

    CpuUtil::bootloader();

    CpuUtil::setDriver(nullptr);

    ASSERT_TRUE(driver.bootloaderCalled);
}
