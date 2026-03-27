# Password Suffix — What It Is and How to Set It Up

## The short version

Your device encrypts everything it stores on the SD card. To do that it needs a key, and that key comes from two things combined:

1. **Your password** — the one you type in when you unlock the device.
2. **The password suffix** — a long random string that gets baked into the firmware itself at compile time.

The suffix is the thing this document is about.

---

## Why does the suffix exist?

Imagine someone steals your SD card but not the device. If the encryption key came *only* from your password, an attacker could just keep guessing passwords on their own computer until one worked.

The suffix makes that impossible. Even if they somehow knew your exact password, they'd *also* need the suffix that's compiled into your specific firmware build — and that's not on the SD card. The two secrets live in completely separate places, so neither one alone is enough to unlock your data.

Think of it like a safe that needs two keys at once. You hold one key (your password), and the other key is physically built into the device.

---

## How it works under the hood

When you unlock the device, the firmware combines your password with the suffix and runs the result through 10,000 iterations of PBKDF2-HMAC-SHA-512 to produce the final encryption key. This key is then used with AES-256 to encrypt and decrypt your stored data. The HMAC signature on each file means the firmware can also detect if data has been tampered with or corrupted.

You don't need to understand any of that — just know that the suffix is a critical part of the key, and without it your data is unreadable.

---

## How to set it up

### Step 1 — Generate a suffix

Run the generator script from the root of the repository:

```bash
python3 utils/gen_password_suffix.py
```

This creates a file called `ks_password_suffix.txt` in the current directory containing something like:

```
_aB3xQ7mNpR2kLwYdT8vCj4Z
```

The script will refuse to run if the file already exists, so you won't accidentally overwrite an existing suffix.

### Step 2 — Back it up immediately

> **This is the most important step.**

If you lose this file and need to reflash or rebuild your firmware, you will permanently lose access to everything stored on your SD card. There is no recovery option.

- Copy it somewhere safe (password manager, encrypted drive, printed paper copy in a secure location — whatever works for you).
- Do **not** commit it to a public git repository or share it anywhere.

### Step 3 — Put the file in your device's build directory

Copy (or move) `ks_password_suffix.txt` into the folder for your target device, e.g.:

```bash
cp ks_password_suffix.txt devices/teensy3-prototype/
```

### Step 4 — Build normally

The build script (`build.sh`) reads the suffix file automatically and passes it into the compiler. You don't need to do anything else:

```bash
cd devices/teensy3-prototype
./build.sh
```

If the file is missing, the build will fail with an error telling you to read this document.

---

## Rebuilding or updating firmware later

As long as you use the **same** `ks_password_suffix.txt`, your existing SD card data will still be readable after a firmware reflash. The suffix just needs to match what was used when the data was first written.

If you use a *different* suffix (or none at all), the firmware will not be able to decrypt any previously stored data.

---

## Summary

| File | What it is | Where it lives |
|---|---|---|
| `ks_password_suffix.txt` | Your unique firmware secret | Device build directory (e.g. `devices/teensy3-prototype/`) |
| `utils/gen_password_suffix.py` | Script that generates it | Repository `utils/` folder |

**Generate it once, back it up, don't lose it.**
