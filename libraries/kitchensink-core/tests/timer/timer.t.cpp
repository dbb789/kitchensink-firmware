#include "gtest/gtest.h"

#include "timer/timer.h"
#include "timer/timermanager.h"
#include "hardware/cpuutil.h"
#include "event/tickevent.h"

#include <utility>

namespace
{

struct MockCpuDriver : public CpuUtil::Driver
{
    uint32_t nowMsResult = 0;

    void bootloader() override { }
    std::size_t freeMemory() override { return 0; }
    uint32_t nowMs() override { return nowMsResult; }
};

}

TEST(Timer, DestructorReleasesTimer)
{
    CpuUtil::setDriver(nullptr);

    TimerManager tm;

    ASSERT_EQ(tm.allocatedTimers(), 0);

    {
        auto timer(tm.createTimer());

        ASSERT_EQ(tm.allocatedTimers(), 1);
    }

    ASSERT_EQ(tm.allocatedTimers(), 0);
}

TEST(Timer, MoveConstructor)
{
    CpuUtil::setDriver(nullptr);

    TimerManager tm;

    auto t1(tm.createTimer());

    ASSERT_EQ(tm.allocatedTimers(), 1);

    auto t2(std::move(t1));

    ASSERT_EQ(tm.allocatedTimers(), 1);
    ASSERT_FALSE(t1.matches(TickEvent::create(0)));
}

TEST(Timer, MoveAssignment)
{
    CpuUtil::setDriver(nullptr);

    TimerManager tm;

    auto t1(tm.createTimer());
    auto t2(tm.createTimer());

    ASSERT_EQ(tm.allocatedTimers(), 2);

    t1 = std::move(t2);

    ASSERT_EQ(tm.allocatedTimers(), 2);
}

TEST(Timer, MatchesOwnTickEvent)
{
    CpuUtil::setDriver(nullptr);

    TimerManager tm;

    auto timer(tm.createTimer());

    ASSERT_TRUE(timer.matches(TickEvent::create(0)));
    ASSERT_FALSE(timer.matches(TickEvent::create(1)));
}

TEST(Timer, NullTimerMatchesNothing)
{
    CpuUtil::setDriver(nullptr);

    TimerManager tm;

    auto t1(tm.createTimer());
    auto t2(std::move(t1));

    ASSERT_FALSE(t1.matches(TickEvent::create(0)));
}

TEST(Timer, ScheduleAddsActiveTimer)
{
    MockCpuDriver driver;
    CpuUtil::setDriver(&driver);

    TimerManager tm;

    auto timer(tm.createTimer());

    ASSERT_EQ(tm.activeTimers(), 0);

    timer.schedule(100);

    ASSERT_EQ(tm.activeTimers(), 1);

    CpuUtil::setDriver(nullptr);
}

TEST(Timer, CancelRemovesActiveTimer)
{
    MockCpuDriver driver;
    CpuUtil::setDriver(&driver);

    TimerManager tm;

    auto timer(tm.createTimer());

    timer.schedule(100);

    ASSERT_EQ(tm.activeTimers(), 1);

    timer.cancel();

    ASSERT_EQ(tm.activeTimers(), 0);

    CpuUtil::setDriver(nullptr);
}
