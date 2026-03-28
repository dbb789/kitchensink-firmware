#include "gtest/gtest.h"

#include "timer/timerutil.h"

TEST(TimerUtil, BasicValue)
{
    uint32_t high(0);
    uint32_t last(0);

    auto result(TimerUtil::toMs64(1000, high, last));

    ASSERT_EQ(result, 1000ULL);
    ASSERT_EQ(high, 0U);
    ASSERT_EQ(last, 1000U);
}

TEST(TimerUtil, NoRolloverAdvancing)
{
    uint32_t high(0);
    uint32_t last(0);

    TimerUtil::toMs64(1000, high, last);
    TimerUtil::toMs64(2000, high, last);
    auto result(TimerUtil::toMs64(3000, high, last));

    ASSERT_EQ(result, 3000ULL);
    ASSERT_EQ(high, 0U);
}

TEST(TimerUtil, RolloverIncrementsHigh)
{
    uint32_t high(0);
    uint32_t last(0xFFFFFFFF);

    auto result(TimerUtil::toMs64(0, high, last));

    ASSERT_EQ(high, 1U);
    ASSERT_EQ(result, 0x100000000ULL);
}

TEST(TimerUtil, RolloverValueCorrect)
{
    uint32_t high(0);
    uint32_t last(0xFFFFFF00);

    auto result(TimerUtil::toMs64(100, high, last));

    ASSERT_EQ(high, 1U);
    ASSERT_EQ(result, 0x100000000ULL + 100);
}

TEST(TimerUtil, MultipleRollovers)
{
    uint32_t high(0);
    uint32_t last(0);

    // First rollover
    TimerUtil::toMs64(0xFFFFFFFF, high, last);
    TimerUtil::toMs64(0, high, last);

    ASSERT_EQ(high, 1U);

    // Second rollover
    TimerUtil::toMs64(0xFFFFFFFF, high, last);
    auto result(TimerUtil::toMs64(0, high, last));

    ASSERT_EQ(high, 2U);
    ASSERT_EQ(result, 0x200000000ULL);
}

TEST(TimerUtil, HighBitsPreservedAfterRollover)
{
    uint32_t high(0);
    uint32_t last(0);

    TimerUtil::toMs64(0xFFFFFFFF, high, last);
    TimerUtil::toMs64(500, high, last);
    auto result(TimerUtil::toMs64(1000, high, last));

    ASSERT_EQ(result, 0x100000000ULL + 1000);
    ASSERT_EQ(high, 1U);
}

TEST(TimerUtil, SameValueDoesNotRollover)
{
    uint32_t high(0);
    uint32_t last(1000);

    auto result(TimerUtil::toMs64(1000, high, last));

    ASSERT_EQ(high, 0U);
    ASSERT_EQ(result, 1000ULL);
}
