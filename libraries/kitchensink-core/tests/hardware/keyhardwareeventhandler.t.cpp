#include "gtest/gtest.h"

#include "hardware/keyhardwareevent.h"
#include "hardware/keyhardwareeventhandler.h"

TEST(KeyHardwareEventHandler, CreateAndDispatch)
{
    int count  = 0;
    int row    = -1;
    int column = -1;
    bool pressed = false;

    auto handler(KeyHardwareEventHandler::create([&](const KeyHardwareEvent& event) {
        ++count;
        row     = event.row;
        column  = event.column;
        pressed = event.pressed;
    }));

    handler.processKeyHardwareEvent(KeyHardwareEvent(2, 3, true));

    ASSERT_EQ(count, 1);
    ASSERT_EQ(row, 2);
    ASSERT_EQ(column, 3);
    ASSERT_TRUE(pressed);
}

TEST(KeyHardwareEventHandler, CreateAndDispatchMultiple)
{
    int callCount = 0;

    auto handler(KeyHardwareEventHandler::create([&](const KeyHardwareEvent&) {
        ++callCount;
    }));

    handler.processKeyHardwareEvent(KeyHardwareEvent(0, 0, true));
    handler.processKeyHardwareEvent(KeyHardwareEvent(1, 1, false));
    handler.processKeyHardwareEvent(KeyHardwareEvent(2, 2, true));

    ASSERT_EQ(callCount, 3);
}

TEST(KeyHardwareEventHandler, DispatchReleaseEvent)
{
    bool pressed = true;

    auto handler(KeyHardwareEventHandler::create([&](const KeyHardwareEvent& event) {
        pressed = event.pressed;
    }));

    handler.processKeyHardwareEvent(KeyHardwareEvent(0, 0, false));

    ASSERT_FALSE(pressed);
}
