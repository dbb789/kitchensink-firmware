# kitchenSink

kitchenSink is a custom keyboard firmware for programmable mechanical
keyboards, written in modern C++14 and built on top of the Arduino
ecosystem.

Unlike other similar custom keyboard firmwares, kitchenSink is
designed to support an on-device UI that runs directly on the host CPU
and allows the user to directly configure the device.

It targets the Teensy 3 series and Adafruit
Feather/Bluefruit boards but should also build on any other Arduino
supported ARM-based board, and supports anything from a split keyboard
layout to an arcade stick controller.

The codebase is structured as a platform-independent core library plus a set of
hardware driver libraries, so adding support for a new board is mostly a matter
of wiring the right drivers together in a new sketch.

---

## Features

**Layers**

Multiple fully independent key mapping layers on a variable sized
grid. Layers are pushed and popped from a stack at runtime, so you can
have momentary layers, toggle layers, or any combination.

**MultiKeys**

Any key can be configured as a MultiKey, firing a different action
depending on how many times it is tapped in quick succession.

**SmartKeys**

SmartKeys give individual keys context-aware behaviour beyond simple press and
release. There are five SmartKey types:

- `Toggle` - press to toggle the key on or off
- `ToggleAutoRelease` - like Toggle, but the key is released when any other key
  is released
- `HoldAutoRelease` - behaves like a normal held key, but releases automatically
  when the next key up event arrives
- `Pair` - press fires one key, release fires a different one
- `HoldOrTap` - acts as a modifier when held, fires a different key when tapped
  alone (the classic space cadet shift pattern)

**Macros**

Recordable macros with three playback modes:

- `Sync` - plays back keypresses one at a time, in sync with the host
- `Realtime` - plays back with the original timing preserved
- `Invert` - plays back a key-up for every key-down and vice versa

Each macro has a name and an optional shortcut string for quick lookup in the
on-device UI.

**Secure Macros**

A separate bank of password-protected macros, encrypted at rest using
AES with HMAC-SHA256 authentication. This is stored on an SD card in
the format used by AES Crypt - https://www.aescrypt.com/

The encryption key is derived from a user password combined with
entropy harvested from keystroke timing, so the device never needs an
external random number source. Secure macros are unlocked on the
device via a password entry screen and locked again on demand.

**On-device UI**

A full menu-driven configuration interface rendered on an attached OLED or TFT
display. Everything is configurable without touching a PC:

- Edit layer key assignments
- Record, edit, name and delete macros
- Configure MultiKeys and SmartKeys
- Adjust global settings (tap delay, key repeat rate, home screen timeout, etc.)
- Various debugging / diagnostic tools

**Auto-repeat**

Configurable key repeat with independent initial delay and repeat rate, driven
by the timer subsystem.

**Persistent storage**

All configuration is serialized to INI format and written to an SD
card on the device.  The default profile is baked into the firmware as
a fallback, and the stored config is loaded on top of it at startup.

---

## Architecture

The firmware is built around a chain of `EventStage` processors. Each stage
receives an event, optionally transforms or absorbs it, and passes it on to the
next stage in the chain. This makes the pipeline easy to reason about and
straightforward to extend.

```
Hardware (I2C matrix / GPIO)
        |
    KeySource          <- translates raw key scans into layer-mapped key events
        |
MultiKeyProcessor      <- handles multi-tap sequences
        |
SmartKeyProcessor      <- applies smart key behaviour
        |
MacroProcessor         <- expands macro trigger keys into macro event sequences
        |
LayerProcessor         <- resolves layer push/pop actions
        |
ActionProcessor        <- dispatches final actions (USB HID, screen events, etc.)
        |
EventManager           <- drives the main event loop, feeds USB output
        |
    USB / BLE output
```

The `ScreenManager` sits alongside this pipeline and handles all on-device UI.
It receives `ScreenEvent`s from the pipeline and manages the screen stack,
launching the appropriate screen for each event.

**Library structure**

```
libraries/
  kitchensink-core/           Core logic: layers, macros, smart keys, UI, crypto
  kitchensink-drivers-teensy/ Teensy USB keyboard and LED source
  kitchensink-drivers-adafruit/ Adafruit Bluefruit BLE keyboard and ILI9431 display
  kitchensink-drivers-generic/ I2C key matrix (MCP23017), GPIO key sets, OLED display

devices/
  teensy3-prototype/          Split keyboard on Teensy 3.1 with I2C matrices and OLED
  feather/                    Feather-based board with ILI9431 TFT display
  arcadestick/                Arcade stick with direct GPIO keys and BLE output
  arcadestick_usb/            Same, but wired USB
```

**Entropy harvesting**

The I2C key matrix scanner feeds keystroke timing data into an `EntropyPool` on
every state change. The pool accumulates samples and produces SHA-256 hashes as
a source of randomness for the crypto subsystem. This is deliberate: the entropy
is derived from the physical timing of key presses, not from any key identities,
so the system cannot act as a key logger.

---

## License

See [LICENSE](LICENSE).
