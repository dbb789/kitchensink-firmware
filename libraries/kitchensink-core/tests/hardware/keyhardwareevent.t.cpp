#include "gtest/gtest.h"

#include "hardware/keyhardwareevent.h"

TEST(KeyHardwareEvent, ConstructPressed)
{
    KeyHardwareEvent ev(3, 5, true);

    ASSERT_EQ(ev.row, 3);
    ASSERT_EQ(ev.column, 5);
    ASSERT_TRUE(ev.pressed);
}

TEST(KeyHardwareEvent, ConstructReleased)
{
    KeyHardwareEvent ev(1, 2, false);

    ASSERT_EQ(ev.row, 1);
    ASSERT_EQ(ev.column, 2);
    ASSERT_FALSE(ev.pressed);
}

TEST(KeyHardwareEvent, ConstructZero)
{
    KeyHardwareEvent ev(0, 0, false);

    ASSERT_EQ(ev.row, 0);
    ASSERT_EQ(ev.column, 0);
    ASSERT_FALSE(ev.pressed);
}
