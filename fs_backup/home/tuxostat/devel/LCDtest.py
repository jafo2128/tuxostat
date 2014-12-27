#!/usr/bin/env python
# test file only

from tuxostat.crystalfontz635usb import crystalfontz635usb as CF635
import serial
import struct
import sys

class tosTester:

    def main(self):

        # clear LED 3 GREEN
        display = CF635(None, "/dev/ttyUSB1")
        #display.clearLED(3)
        #display.setLED(2, 1, 0)
        #display.writeLineFromLeft(0, "Hi")
        display.setBacklight(0)
        sys.exit(1)

if __name__ == '__main__':
    myclass = tosTester()
    myclass.main()
