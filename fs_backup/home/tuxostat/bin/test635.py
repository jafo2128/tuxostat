#!/usr/bin/env python
# Time-stamp: "2008-06-09 23:18:15 jantman"

from crystalfontz635usb import crystalfontz635usb as CF635
import time

display = CF635(None, "/dev/ttyUSB0") # init the display

# clear everything
print "Clearing LEDs..."
display.clearAllLEDs()
time.sleep(display.WAIT_TIME)

print "Clearing Screen..."
display.clearScreen()
time.sleep(display.WAIT_TIME)

#display.setBacklight(0)

#display.setContrast(95)

"""
print "Setting LED 0 green level 50..."
display.setLED(0, 0, 50) # color 0 grn 1 red , lvl 50
time.sleep(display.WAIT_TIME)
"""

print "Setting LED 0 green level 50..."
display.setLED(0, 0, 0) # color 0 grn 1 red , lvl 50
time.sleep(display.WAIT_TIME)

print "Setting LED 0 red level 50..."
display.setLED(0, 1, 50) # color 0 grn 1 red , lvl 50
time.sleep(display.WAIT_TIME)

print "setting line 0 to \"tuxOstat\""
display.writeCenteredLine(0, "tuxOstat")
time.sleep(display.WAIT_TIME)

print "setting line 1 to \"TESTING\""
display.writeCenteredLine(1, "TESTING")
time.sleep(display.WAIT_TIME)

print "setting line 2 to \"updated:\""
display.writeCenteredLine(2, "updated:")
time.sleep(display.WAIT_TIME)

datestr = time.strftime("%Y-%m-%dT%H:%M:%S")
print "setting line 3 to \""+datestr+"\""
display.writeCenteredLine(3, datestr)
time.sleep(display.WAIT_TIME)
