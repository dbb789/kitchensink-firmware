#!/bin/bash

arduino-cli compile -b adafruit:samd:adafruit_feather_m0 --libraries ../../libraries --build-property 'build.extra_flags=-DKS_PASSWORD_SUFFIX="_unused"' --warnings all $@ ./arcadestick.ino
