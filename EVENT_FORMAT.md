# Event Format Reference

This document describes the serialized format of events as produced and consumed by `EventSerializer`.

---

## Prefix modifier

| Token | Meaning |
|-------|---------|
| `!`   | Inverted event — prepend to any event format below |

---

## Event types

### `_` — Empty / unknown

Represents a null or unrecognised event.

---

### `A<n>` — Action event

Triggers a built-in action.

| Field | Description |
|-------|-------------|
| `n`   | Integer action type index |

**Examples:** `A0`, `A3`

---

### `D<ms>` — Delay event

Introduces a delay in a macro sequence.

| Field | Description |
|-------|-------------|
| `ms`  | Delay duration in milliseconds |

**Examples:** `D100`, `D500`

---

### `K<name>` — Key event

Represents a HID key press or release.

| Field  | Description |
|--------|-------------|
| `name` | Named key code (e.g. `KC_A`, `KC_ENTER`) — or `_<n>` for an unnamed key by numeric index |

**Examples:** `KKC_A`, `KKC_ENTER`, `K_42`

---

### `L<n>` — Layer event

Switches to a keyboard layer.

| Field | Description |
|-------|-------------|
| `n`   | Layer index (e.g. `0` = Layer 0) |

**Examples:** `L0`, `L1`, `L3`

---

### `M<type><n>` — Macro event

Triggers a macro by ID.

| Field  | Description |
|--------|-------------|
| `type` | `D` = default macro, `S` = secure macro |
| `n`    | Macro ID |

**Examples:** `MD0`, `MD5`, `MS2`

---

### `N<n>` — Multi event

Triggers a multi-action by ID.

| Field | Description |
|-------|-------------|
| `n`   | Multi ID |

**Examples:** `N0`, `N4`

---

### `U<type>:<n>` — Screen event

Navigates to or controls a UI screen.

| Field  | Description |
|--------|-------------|
| `type` | Integer screen event type |
| `n`    | Screen index |

**Examples:** `U0:0`, `U1:2`

---

### `S<n>` — Smart event

Triggers a smart key action by ID.

| Field | Description |
|-------|-------------|
| `n`   | Smart ID |

**Examples:** `S0`, `S7`

---

## Human-readable format

`EventSerializer::serializeReadable()` produces a longer, display-friendly form of each event. These are not parsed back — they are for logging and UI display only.

| Serialized | Readable example |
|------------|-----------------|
| `A2`       | `Action 2` |
| `D100`     | `Delay 100` |
| `KKC_A`    | `KC_A` |
| `L1`       | `Layer 1` |
| `MD3`      | `Macro 3` |
| `MS1`      | `Secure Macro 1` |
| `N2`       | `Multi 2` |
| `U1:0`     | `Screen 1:0` |
| `S4`       | `Smart 4` |
| *(internal)* | `Tick <id>` |
| *(internal)* | `LED NCS` (N=NumLock, C=CapsLock, S=ScrollLock, `-` if off) |
