#!/usr/bin/env python
#  Time-stamp: "2008-06-10 16:35:08 tuxostat"
#  $Id: backlightTest.py,v 1.1.1.1 2008/06/11 16:52:16 jantman Exp $

# IMPORTS
from tuxostat.crystalfontz635usb import crystalfontz635usb as CF635

display = CF635(None, "/dev/ttyUSB0") # init the display

display.setBacklight(50)



