#include "gtest/gtest.h"

#include "timer/timermanager.h"
#include "hardware/cpuutil.h"
#include "event/event.h"
#include "event/eventstage.h"
#include "event/tickevent.h"

#include <vector>

namespace
{

struct MockCpuDriver : public CpuUtil::Driver
{
    uint32_t nowMsResult = 0;

    void bootloader() override { }
    std::size_t freeMemory() override { return 0; }
    uint32_t nowMs() override { return nowMsResult; }
};

struct MockEventStage : public EventStage
{
    std::vector<Event> events;

    bool processEvent(const Event& event) override
    {
        events.push_back(event);
        return true;
    }
};

}

TEST(TimerManager, Default)
{
    TimerManager tm;

    ASSERT_EQ(tm.allocatedTimers(), 0);
    ASSERT_EQ(tm.activeTimers(), 0);
}

TEST(TimerManager, CreateTimerIncreasesAllocated)
{
    CpuUtil::setDriver(nullptr);

    TimerManager tm;

    auto timer(tm.createTimer());

    ASSERT_EQ(tm.allocatedTimers(), 1);
}

TEST(TimerManager, CreateMultipleTimers)
{
    CpuUtil::setDriver(nullptr);

    TimerManager tm;

    auto t1(tm.createTimer());
    auto t2(tm.createTimer());
    auto t3(tm.createTimer());

    ASSERT_EQ(tm.allocatedTimers(), 3);
}

TEST(TimerManager, ReleaseOnDestruction)
{
    CpuUtil::setDriver(nullptr);

    TimerManager tm;

    {
        auto t1(tm.createTimer());
        auto t2(tm.createTimer());

        ASSERT_EQ(tm.allocatedTimers(), 2);
    }

    ASSERT_EQ(tm.allocatedTimers(), 0);
}

TEST(TimerManager, ScheduleIncreasesActiveTimers)
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

TEST(TimerManager, PollEventDoesNotFireEarly)
{
    MockCpuDriver driver;
    CpuUtil::setDriver(&driver);

    TimerManager tm;

    auto timer(tm.createTimer());

    timer.schedule(100);

    MockEventStage stage;

    driver.nowMsResult = 99;
    tm.pollEvent(stage);

    ASSERT_EQ(stage.events.size(), 0);

    CpuUtil::setDriver(nullptr);
}

TEST(TimerManager, PollEventFires)
{
    MockCpuDriver driver;
    CpuUtil::setDriver(&driver);

    TimerManager tm;

    auto timer(tm.createTimer());

    timer.schedule(100);

    MockEventStage stage;

    driver.nowMsResult = 100;
    tm.pollEvent(stage);

    ASSERT_EQ(stage.events.size(), 1);
    ASSERT_TRUE(timer.matches(stage.events[0]));

    CpuUtil::setDriver(nullptr);
}

TEST(TimerManager, PollEventDoesNotFireTwice)
{
    MockCpuDriver driver;
    CpuUtil::setDriver(&driver);

    TimerManager tm;

    auto timer(tm.createTimer());

    timer.schedule(100);

    MockEventStage stage;

    driver.nowMsResult = 100;
    tm.pollEvent(stage);
    tm.pollEvent(stage);

    ASSERT_EQ(stage.events.size(), 1);

    CpuUtil::setDriver(nullptr);
}

TEST(TimerManager, CancelPreventsFiring)
{
    MockCpuDriver driver;
    CpuUtil::setDriver(&driver);

    TimerManager tm;

    auto timer(tm.createTimer());

    timer.schedule(100);
    timer.cancel();

    MockEventStage stage;

    driver.nowMsResult = 100;
    tm.pollEvent(stage);

    ASSERT_EQ(stage.events.size(), 0);

    CpuUtil::setDriver(nullptr);
}

TEST(TimerManager, ScheduleRepeatingFiresMultipleTimes)
{
    MockCpuDriver driver;
    CpuUtil::setDriver(&driver);

    TimerManager tm;

    auto timer(tm.createTimer());

    timer.scheduleRepeating(100, 50);

    MockEventStage stage;

    driver.nowMsResult = 100;
    tm.pollEvent(stage);

    ASSERT_EQ(stage.events.size(), 1);
    ASSERT_EQ(tm.activeTimers(), 1);

    driver.nowMsResult = 150;
    tm.pollEvent(stage);

    ASSERT_EQ(stage.events.size(), 2);
    ASSERT_EQ(tm.activeTimers(), 1);

    driver.nowMsResult = 200;
    tm.pollEvent(stage);

    ASSERT_EQ(stage.events.size(), 3);

    ASSERT_TRUE(timer.matches(stage.events[0]));
    ASSERT_TRUE(timer.matches(stage.events[1]));
    ASSERT_TRUE(timer.matches(stage.events[2]));

    CpuUtil::setDriver(nullptr);
}

TEST(TimerManager, ActiveTimersDecreasesAfterFire)
{
    MockCpuDriver driver;
    CpuUtil::setDriver(&driver);

    TimerManager tm;

    auto timer(tm.createTimer());

    timer.schedule(100);

    ASSERT_EQ(tm.activeTimers(), 1);

    MockEventStage stage;

    driver.nowMsResult = 100;
    tm.pollEvent(stage);

    ASSERT_EQ(tm.activeTimers(), 0);

    CpuUtil::setDriver(nullptr);
}
