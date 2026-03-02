#!/bin/bash

arduino-cli compile -b teensy:avr:teensy31:usb=hid,speed=48,opt=oslto,keys=en-us --libraries ../../libraries --warnings all $@ teensy3-prototype.ino && arduino-cli upload -b teensy:avr:teensy31 -p usb7/7-1/7-1.1
