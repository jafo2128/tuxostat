#!/usr/bin/env python
# test file only

from tuxostat.crystalfontz635usb import crystalfontz635usb as CF635
import serial
import struct
import sys

class tosTester:

    def main(self):

        # clear LED 3 GREEN
        #display = CF635(None, "/dev/ttyUSB1")
        #display.clearLED(3)
        #sys.exit(1)

        
        self.conn = serial.Serial("/dev/ttyUSB1", 115200)


        structFmt = "BBsH"
        command = 0x0E
        dataLen = 0x01
        data = chr(0x32)
        CRC = 0xDD9E

        # set LED 3 red
        #buffer = struct.pack("BB2sH", 0x22, 0x02, chr(0x08)+chr(0x32), 0x2312)

        # write "Hi" to line 1
        # WORKING
        # buffer = struct.pack("BB4sH", 0x1F, 4, chr(0x0)+chr(0x0)+"H"+"i", 0xD3C7)
        
	buffer = struct.pack("BBcH", 0x0E, 0x01, chr(0x32), 0x9EDD)

        val = 0x15
        s = chr(0x0E)+chr(0x01)+chr(val)
        display = CF635(None, "/dev/ttyUSB1")
        CRC = display.getCRC(s)
        print "CRC="+hex(CRC)
        CRCs = struct.pack("H", CRC)
        final = s+CRCs

	self.conn.write(final)

if __name__ == '__main__':
    myclass = tosTester()
    myclass.main()
