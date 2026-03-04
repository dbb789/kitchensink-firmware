#include "gtest/gtest.h"

#include "hardware/debounce.h"

#include "types/bitmask.h"

TEST(Debounce, Default)
{
    Debounce<Bitmask<4>> d;

    ASSERT_TRUE(d.state.empty());
    ASSERT_TRUE(d.delta.empty());
}

TEST(Debounce, NoChangeWithinLatency)
{
    Debounce<Bitmask<4>> d;

    Bitmask<4> next;
    next[0] = true;

    ASSERT_FALSE(d.process(0, next));
    ASSERT_FALSE(d.process(6, next));
}

TEST(Debounce, KeyPress)
{
    Debounce<Bitmask<4>> d;

    Bitmask<4> next;
    next[0] = true;

    ASSERT_FALSE(d.process(0, next));
    ASSERT_FALSE(d.process(7, next));
    ASSERT_TRUE(d.process(14, next));

    ASSERT_FALSE(d.state.empty());
    ASSERT_TRUE(d.state[0]);
    ASSERT_FALSE(d.state[1]);
    ASSERT_TRUE(d.delta[0]);
}

TEST(Debounce, KeyRelease)
{
    Debounce<Bitmask<4>> d;

    Bitmask<4> pressed;
    pressed[0] = true;

    Bitmask<4> released;

    d.process(0, pressed);
    d.process(7, pressed);
    d.process(14, pressed);

    ASSERT_FALSE(d.state.empty());

    ASSERT_TRUE(d.process(21, released));

    ASSERT_TRUE(d.state.empty());
    ASSERT_TRUE(d.delta[0]);
}

TEST(Debounce, DebounceFiltersNoise)
{
    Debounce<Bitmask<4>> d;

    Bitmask<4> pressed;
    pressed[0] = true;

    Bitmask<4> released;

    d.process(0, pressed);
    d.process(5, released);

    ASSERT_FALSE(d.process(7, released));

    ASSERT_TRUE(d.state.empty());
}

TEST(Debounce, MultipleKeys)
{
    Debounce<Bitmask<4>> d;

    Bitmask<4> next;
    next[0] = true;
    next[2] = true;

    d.process(0, next);
    d.process(7, next);
    d.process(14, next);

    ASSERT_TRUE(d.state[0]);
    ASSERT_FALSE(d.state[1]);
    ASSERT_TRUE(d.state[2]);
    ASSERT_FALSE(d.state[3]);
}
