#!/bin/bash

KS_PASSWORD_SUFFIX_FILE="./ks_password_suffix.txt"

if ! [ -f ./ks_password_suffix.txt ]; then
    echo "A password suffix must be configured to build this library. Please read PASSWORD_SUFFIX.md for more information."
    exit 1
fi

KS_PASSWORD_SUFFIX=`head -n 1 "${KS_PASSWORD_SUFFIX_FILE}" | sed 's/^[[:blank:]]*//;s/[[:blank:]]*$//'`

if [ -z "$KS_PASSWORD_SUFFIX" ]; then
    echo "Password suffix file does not define a password suffix."
    exit 1
fi

# Workaround for teensy not supporting build.extra_flags etc
BUILD_FLAGS=`arduino-cli compile -b teensy:avr:teensy31:usb=hid,speed=48,opt=oslto,keys=en-us --show-properties | grep 'build.flags.cpp' | cut -c 17-`
BUILD_FLAGS="${BUILD_FLAGS} -DKS_PASSWORD_SUFFIX=\"${KS_PASSWORD_SUFFIX}\""

arduino-cli compile -b teensy:avr:teensy31:usb=hid,speed=48,opt=oslto,keys=en-us --build-property "build.flags.cpp=${BUILD_FLAGS}" --libraries ../../libraries --warnings all $@ teensy3-prototype.ino
