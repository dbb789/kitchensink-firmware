#include "gtest/gtest.h"

#include "hardware/keyboardplate.h"
#include "hardware/keyhardwareeventhandler.h"

#include "types/bitmask2d.h"

#include <array>
#include <vector>

struct MockKeyDriver
{
    Bitmask2d<2, 2> scanResult;

    void scan(Bitmask2d<2, 2>& state)
    {
        state = scanResult;
    }
};

TEST(KeyboardPlate, AnyPressedFalseInitially)
{
    MockKeyDriver driver;

    std::array<uint8_t, 2> cols = {{0, 1}};
    std::array<uint8_t, 2> rows = {{0, 1}};

    KeyboardPlate<MockKeyDriver, 2, 2> plate(driver, cols, rows);

    ASSERT_FALSE(plate.anyPressed());
}

TEST(KeyboardPlate, CurrentlyPressedNoEventsInitially)
{
    MockKeyDriver driver;

    std::array<uint8_t, 2> cols = {{0, 1}};
    std::array<uint8_t, 2> rows = {{0, 1}};

    KeyboardPlate<MockKeyDriver, 2, 2> plate(driver, cols, rows);

    int callCount = 0;

    auto handler(KeyHardwareEventHandler::create([&](const KeyHardwareEvent&) {
        ++callCount;
    }));

    plate.currentlyPressed(handler);

    ASSERT_EQ(callCount, 0);
}

TEST(KeyboardPlate, PollDispatchesKeyPress)
{
    MockKeyDriver driver;

    std::array<uint8_t, 2> cols = {{10, 11}};
    std::array<uint8_t, 2> rows = {{20, 21}};

    KeyboardPlate<MockKeyDriver, 2, 2> plate(driver, cols, rows);

    driver.scanResult[0][0] = true;

    std::vector<KeyHardwareEvent> events;

    auto handler(KeyHardwareEventHandler::create([&](const KeyHardwareEvent& ev) {
        events.push_back(ev);
    }));

    plate.poll(0, handler);
    plate.poll(7, handler);
    plate.poll(14, handler);

    ASSERT_EQ(events.size(), 1);
    ASSERT_EQ(events[0].row, 20);
    ASSERT_EQ(events[0].column, 10);
    ASSERT_TRUE(events[0].pressed);
}

TEST(KeyboardPlate, AnyPressedAfterKeyPress)
{
    MockKeyDriver driver;

    std::array<uint8_t, 2> cols = {{0, 1}};
    std::array<uint8_t, 2> rows = {{0, 1}};

    KeyboardPlate<MockKeyDriver, 2, 2> plate(driver, cols, rows);

    driver.scanResult[0][0] = true;

    auto handler(KeyHardwareEventHandler::create([](const KeyHardwareEvent&) {}));

    plate.poll(0, handler);
    plate.poll(7, handler);
    plate.poll(14, handler);

    ASSERT_TRUE(plate.anyPressed());
}

TEST(KeyboardPlate, PollDispatchesKeyRelease)
{
    MockKeyDriver driver;

    std::array<uint8_t, 2> cols = {{10, 11}};
    std::array<uint8_t, 2> rows = {{20, 21}};

    KeyboardPlate<MockKeyDriver, 2, 2> plate(driver, cols, rows);

    driver.scanResult[0][0] = true;

    auto handler(KeyHardwareEventHandler::create([](const KeyHardwareEvent&) {}));

    plate.poll(0, handler);
    plate.poll(7, handler);
    plate.poll(14, handler);

    driver.scanResult[0][0] = false;

    std::vector<KeyHardwareEvent> events;

    auto releaseHandler(KeyHardwareEventHandler::create([&](const KeyHardwareEvent& ev) {
        events.push_back(ev);
    }));

    plate.poll(21, releaseHandler);

    ASSERT_EQ(events.size(), 1);
    ASSERT_EQ(events[0].row, 20);
    ASSERT_EQ(events[0].column, 10);
    ASSERT_FALSE(events[0].pressed);
}
