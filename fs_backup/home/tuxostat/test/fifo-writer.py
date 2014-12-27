#!/usr/bin/env python

# test of FIFO / named pipe writing
# Time-stamp: "2008-06-13 22:42:38 tuxostat"
# $Id: fifo-writer.py,v 1.1.1.1 2008/06/15 16:03:25 jantman Exp $

import os

fifoFile = "/var/local/tuxostat/tos-LCDd-fifo"

fifo = open(fifoFile, "a")# Mode "a" or "w"

try:
    fifo.write("This string goes down the pipe.\n")
    # Newline is important because the listener uses readline().
    fifo.flush()
    # Output is buffered.
except IOError:
    pass

print "Done writing."
