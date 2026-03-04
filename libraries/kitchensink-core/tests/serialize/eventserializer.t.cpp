#include "gtest/gtest.h"

#include "types/strbuf.h"
#include "types/strref.h"
#include "types/stroutstream.h"
#include "event/actionevent.h"
#include "event/delayevent.h"
#include "event/keyevent.h"
#include "event/layerevent.h"
#include "event/macroevent.h"
#include "event/multievent.h"
#include "event/screenevent.h"
#include "event/smartevent.h"
#include "data/keycode.h"
#include "serialize/eventserializer.h"

TEST(EventSerializer, SerializeEmpty)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(Event(), os);

    ASSERT_EQ(os.str(), "_");
}

TEST(EventSerializer, SerializeActionEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(ActionEvent::create(ActionEvent::Type::kBootloader), os);

    ASSERT_EQ(os.str(), "A0");
}

TEST(EventSerializer, SerializeActionEventLockSecureMacros)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(ActionEvent::create(ActionEvent::Type::kLockSecureMacros), os);

    ASSERT_EQ(os.str(), "A2");
}

TEST(EventSerializer, SerializeDelayEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(DelayEvent::create(100), os);

    ASSERT_EQ(os.str(), "D100");
}

TEST(EventSerializer, SerializeKeyEventNamed)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(KeyEvent::create(KeyCode::A), os);

    ASSERT_EQ(os.str(), "KA");
}

TEST(EventSerializer, SerializeKeyEventEnter)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(KeyEvent::create(KeyCode::Enter), os);

    ASSERT_EQ(os.str(), "KEnter");
}

TEST(EventSerializer, SerializeKeyEventNumeric)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    // KeyCode 0xB0 is outside the named range and not a modifier
    EventSerializer::serialize(KeyEvent::create(static_cast<KeyCode>(0xB0)), os);

    ASSERT_EQ(os.str(), "K_176");
}

TEST(EventSerializer, SerializeLayerEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(LayerEvent::create(1), os);

    ASSERT_EQ(os.str(), "L1");
}

TEST(EventSerializer, SerializeMacroEventDefault)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(MacroEvent::create(MacroEvent::Type::kDefault, 5), os);

    ASSERT_EQ(os.str(), "MD5");
}

TEST(EventSerializer, SerializeMacroEventSecure)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(MacroEvent::create(MacroEvent::Type::kSecure, 3), os);

    ASSERT_EQ(os.str(), "MS3");
}

TEST(EventSerializer, SerializeMultiEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(MultiEvent::create(2), os);

    ASSERT_EQ(os.str(), "N2");
}

TEST(EventSerializer, SerializeScreenEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(ScreenEvent::create(ScreenEvent::Type::kMenu, 0), os);

    ASSERT_EQ(os.str(), "U1:0");
}

TEST(EventSerializer, SerializeScreenEventWithIndex)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(ScreenEvent::create(ScreenEvent::Type::kScreen, 3), os);

    ASSERT_EQ(os.str(), "U2:3");
}

TEST(EventSerializer, SerializeSmartEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(SmartEvent::create(3), os);

    ASSERT_EQ(os.str(), "S3");
}

TEST(EventSerializer, SerializeInvertedLayerEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(LayerEvent::create(2).invert(), os);

    ASSERT_EQ(os.str(), "!L2");
}

TEST(EventSerializer, SerializeInvertedKeyEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(KeyEvent::create(KeyCode::Space).invert(), os);

    ASSERT_EQ(os.str(), "!KSpace");
}

TEST(EventSerializer, DeserializeActionEvent)
{
    Event event;

    EventSerializer::deserialize("A0", event);

    ASSERT_TRUE(event.is<ActionEvent>());
    ASSERT_EQ(event.get<ActionEvent>().type, ActionEvent::Type::kBootloader);
}

TEST(EventSerializer, DeserializeDelayEvent)
{
    Event event;

    EventSerializer::deserialize("D100", event);

    ASSERT_TRUE(event.is<DelayEvent>());
    ASSERT_EQ(event.get<DelayEvent>().delayMs, 100);
}

TEST(EventSerializer, DeserializeKeyEventNamed)
{
    Event event;

    EventSerializer::deserialize("KA", event);

    ASSERT_TRUE(event.is<KeyEvent>());
    ASSERT_EQ(event.get<KeyEvent>().key, KeyCode::A);
}

TEST(EventSerializer, DeserializeKeyEventNumeric)
{
    Event event;

    EventSerializer::deserialize("K_176", event);

    ASSERT_TRUE(event.is<KeyEvent>());
    ASSERT_EQ(event.get<KeyEvent>().key, static_cast<KeyCode>(0xB0));
}

TEST(EventSerializer, DeserializeLayerEvent)
{
    Event event;

    EventSerializer::deserialize("L1", event);

    ASSERT_TRUE(event.is<LayerEvent>());
    ASSERT_EQ(event.get<LayerEvent>().layer, 1);
}

TEST(EventSerializer, DeserializeMacroEventDefault)
{
    Event event;

    EventSerializer::deserialize("MD5", event);

    ASSERT_TRUE(event.is<MacroEvent>());
    ASSERT_EQ(event.get<MacroEvent>().type, MacroEvent::Type::kDefault);
    ASSERT_EQ(event.get<MacroEvent>().macroId, 5);
}

TEST(EventSerializer, DeserializeMacroEventSecure)
{
    Event event;

    EventSerializer::deserialize("MS3", event);

    ASSERT_TRUE(event.is<MacroEvent>());
    ASSERT_EQ(event.get<MacroEvent>().type, MacroEvent::Type::kSecure);
    ASSERT_EQ(event.get<MacroEvent>().macroId, 3);
}

TEST(EventSerializer, DeserializeMultiEvent)
{
    Event event;

    EventSerializer::deserialize("N2", event);

    ASSERT_TRUE(event.is<MultiEvent>());
    ASSERT_EQ(event.get<MultiEvent>().multiId, 2);
}

TEST(EventSerializer, DeserializeScreenEvent)
{
    Event event;

    EventSerializer::deserialize("U1:0", event);

    ASSERT_TRUE(event.is<ScreenEvent>());
    ASSERT_EQ(event.get<ScreenEvent>().type, ScreenEvent::Type::kMenu);
    ASSERT_EQ(event.get<ScreenEvent>().index, 0);
}

TEST(EventSerializer, DeserializeSmartEvent)
{
    Event event;

    EventSerializer::deserialize("S3", event);

    ASSERT_TRUE(event.is<SmartEvent>());
    ASSERT_EQ(event.get<SmartEvent>().smartId, 3);
}

TEST(EventSerializer, DeserializeInverted)
{
    Event event;

    EventSerializer::deserialize("!L2", event);

    ASSERT_TRUE(event.is<LayerEvent>());
    ASSERT_TRUE(event.inverted());
    ASSERT_EQ(event.get<LayerEvent>().layer, 2);
}

TEST(EventSerializer, DeserializeEmptyString)
{
    Event expected;
    Event event;

    EventSerializer::deserialize("", event);

    ASSERT_EQ(event, expected);
}

TEST(EventSerializer, RoundTripActionEvent)
{
    const Event original(ActionEvent::create(ActionEvent::Type::kDumpEntropyPool));

    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(original, os);

    Event result;

    EventSerializer::deserialize(os.str(), result);

    ASSERT_EQ(result, original);
}

TEST(EventSerializer, RoundTripKeyEvent)
{
    const Event original(KeyEvent::create(KeyCode::Enter));

    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(original, os);

    Event result;

    EventSerializer::deserialize(os.str(), result);

    ASSERT_EQ(result, original);
}

TEST(EventSerializer, RoundTripLayerEvent)
{
    const Event original(LayerEvent::create(3));

    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(original, os);

    Event result;

    EventSerializer::deserialize(os.str(), result);

    ASSERT_EQ(result, original);
}

TEST(EventSerializer, RoundTripInvertedLayerEvent)
{
    const Event original(LayerEvent::create(3).invert());

    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(original, os);

    Event result;

    EventSerializer::deserialize(os.str(), result);

    ASSERT_EQ(result, original);
}

TEST(EventSerializer, RoundTripMacroEventSecure)
{
    const Event original(MacroEvent::create(MacroEvent::Type::kSecure, 7));

    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(original, os);

    Event result;

    EventSerializer::deserialize(os.str(), result);

    ASSERT_EQ(result, original);
}

TEST(EventSerializer, RoundTripScreenEvent)
{
    const Event original(ScreenEvent::create(ScreenEvent::Type::kEditMacro, 2));

    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serialize(original, os);

    Event result;

    EventSerializer::deserialize(os.str(), result);

    ASSERT_EQ(result, original);
}

TEST(EventSerializerReadable, SerializeKeyEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serializeReadable(KeyEvent::create(KeyCode::A), os);

    ASSERT_EQ(os.str(), "A");
}

TEST(EventSerializerReadable, SerializeDelayEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serializeReadable(DelayEvent::create(50), os);

    ASSERT_EQ(os.str(), "Delay 50");
}

TEST(EventSerializerReadable, SerializeLayerEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serializeReadable(LayerEvent::create(1), os);

    ASSERT_EQ(os.str(), "Layer 1");
}

TEST(EventSerializerReadable, SerializeMacroEventDefault)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serializeReadable(MacroEvent::create(MacroEvent::Type::kDefault, 4), os);

    ASSERT_EQ(os.str(), "Macro 4");
}

TEST(EventSerializerReadable, SerializeMacroEventSecure)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serializeReadable(MacroEvent::create(MacroEvent::Type::kSecure, 4), os);

    ASSERT_EQ(os.str(), "Secure Macro 4");
}

TEST(EventSerializerReadable, SerializeActionEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serializeReadable(ActionEvent::create(ActionEvent::Type::kBootloader), os);

    ASSERT_EQ(os.str(), "Action 0");
}

TEST(EventSerializerReadable, SerializeSmartEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serializeReadable(SmartEvent::create(5), os);

    ASSERT_EQ(os.str(), "Smart 5");
}

TEST(EventSerializerReadable, SerializeScreenEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serializeReadable(ScreenEvent::create(ScreenEvent::Type::kMenu, 0), os);

    ASSERT_EQ(os.str(), "Screen 1:0");
}

TEST(EventSerializerReadable, SerializeMultiEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serializeReadable(MultiEvent::create(2), os);

    ASSERT_EQ(os.str(), "Multi 2");
}

TEST(EventSerializerReadable, SerializeInvertedEvent)
{
    StrBuf<32> buf;
    StrOutStream os(buf);

    EventSerializer::serializeReadable(LayerEvent::create(1).invert(), os);

    ASSERT_EQ(os.str(), "!Layer 1");
}
