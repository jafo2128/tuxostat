#!/usr/bin/env python

# test of FIFO / named pipe reading
# Time-stamp: "2008-06-13 23:08:33 tuxostat"
# $Id: fifo-reader.py,v 1.1.1.1 2008/06/15 16:03:29 jantman Exp $

import os

fifoFile = "/var/local/tuxostat/tos-LCDd-fifo"

os.system("mkfifo -m 666 "+fifoFile) # make the fifo

fifo = open(fifoFile,"r")

while True:
    data = fifo.readline()[:-1]
    # [:-1] because of the newline added by print.
    if data != "":
        print data

print "Done"

