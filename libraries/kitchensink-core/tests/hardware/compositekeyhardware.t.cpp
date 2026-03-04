#include "gtest/gtest.h"

#include "hardware/compositekeyhardware.h"
#include "hardware/keyhardwareeventhandler.h"

struct MockKeyHardware : public KeyHardware
{
    int  pollCount             = 0;
    int  currentlyPressedCount = 0;
    bool pressed               = false;

    void poll(uint32_t, const KeyHardwareEventHandler&) override
    {
        ++pollCount;
    }

    void currentlyPressed(const KeyHardwareEventHandler&) override
    {
        ++currentlyPressedCount;
    }

    bool anyPressed() const override
    {
        return pressed;
    }
};

TEST(CompositeKeyHardware, PollForwardsToAllEntries)
{
    MockKeyHardware a;
    MockKeyHardware b;

    CompositeKeyHardware<2>::Entries entries = {{&a, &b}};
    CompositeKeyHardware<2> composite(entries);

    auto handler(KeyHardwareEventHandler::create([](const KeyHardwareEvent&) {}));

    composite.poll(0, handler);

    ASSERT_EQ(a.pollCount, 1);
    ASSERT_EQ(b.pollCount, 1);
}

TEST(CompositeKeyHardware, CurrentlyPressedForwardsToAllEntries)
{
    MockKeyHardware a;
    MockKeyHardware b;

    CompositeKeyHardware<2>::Entries entries = {{&a, &b}};
    CompositeKeyHardware<2> composite(entries);

    auto handler(KeyHardwareEventHandler::create([](const KeyHardwareEvent&) {}));

    composite.currentlyPressed(handler);

    ASSERT_EQ(a.currentlyPressedCount, 1);
    ASSERT_EQ(b.currentlyPressedCount, 1);
}

TEST(CompositeKeyHardware, AnyPressedFalseWhenNonePressed)
{
    MockKeyHardware a;
    MockKeyHardware b;

    CompositeKeyHardware<2>::Entries entries = {{&a, &b}};
    CompositeKeyHardware<2> composite(entries);

    ASSERT_FALSE(composite.anyPressed());
}

TEST(CompositeKeyHardware, AnyPressedTrueWhenOnePressed)
{
    MockKeyHardware a;
    MockKeyHardware b;

    b.pressed = true;

    CompositeKeyHardware<2>::Entries entries = {{&a, &b}};
    CompositeKeyHardware<2> composite(entries);

    ASSERT_TRUE(composite.anyPressed());
}

TEST(CompositeKeyHardware, AnyPressedTrueWhenAllPressed)
{
    MockKeyHardware a;
    MockKeyHardware b;

    a.pressed = true;
    b.pressed = true;

    CompositeKeyHardware<2>::Entries entries = {{&a, &b}};
    CompositeKeyHardware<2> composite(entries);

    ASSERT_TRUE(composite.anyPressed());
}

TEST(CompositeKeyHardware, PollCalledMultipleTimes)
{
    MockKeyHardware a;
    MockKeyHardware b;
    MockKeyHardware c;

    CompositeKeyHardware<3>::Entries entries = {{&a, &b, &c}};
    CompositeKeyHardware<3> composite(entries);

    auto handler(KeyHardwareEventHandler::create([](const KeyHardwareEvent&) {}));

    composite.poll(100, handler);
    composite.poll(200, handler);

    ASSERT_EQ(a.pollCount, 2);
    ASSERT_EQ(b.pollCount, 2);
    ASSERT_EQ(c.pollCount, 2);
}
