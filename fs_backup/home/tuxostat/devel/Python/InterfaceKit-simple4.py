#!/usr/bin/env python

# jantman
# Time-stamp: "2008-06-09 16:12:34 tuxostat"

#Basic imports
from threading import *
from ctypes import *
import sys
#Phidget specific imports
from Phidgets.PhidgetException import *
from Phidgets.Events.Events import *
from Phidgets.Devices.InterfaceKit import *
import time

#Create an interfacekit object
interfaceKit = InterfaceKit()

#Information Display Function
def displayDeviceInfo():
    print "|------------|----------------------------------|--------------|------------|"
    print "|- Attached -|-              Type              -|- Serial No. -|-  Version -|"
    print "|------------|----------------------------------|--------------|------------|"
    print "|- %8s -|- %30s -|- %10d -|- %8d -|" % (interfaceKit.isAttached(), interfaceKit.getDeviceType(), interfaceKit.getSerialNum(), interfaceKit.getDeviceVersion())
    print "|------------|----------------------------------|--------------|------------|"
    return 0


print "Opening phidget object...."

try:
    interfaceKit.openPhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Waiting for attach...."

try:
    interfaceKit.waitForAttach(10000)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    try:
        interfaceKit.closePhidget()
    except PhidgetException, e:
        print "Phidget Exception %i: %s" % (e.code, e.message)
        print "Exiting...."
        exit(1)
    print "Exiting...."
    exit(1)


print "Output Count: "+str(interfaceKit.getOutputCount())

time.sleep(3)

print "Output 0 is currently in state "+str(interfaceKit.getOutputState(0))
print "Press Enter to toggle output 0"
chr = sys.stdin.read(1)
if interfaceKit.getOutputState(0) is False:
    print "setting output dtate to True"
    interfaceKit.setOutputState(0, True)
    print "done"
else:
    print "setting output state to False"
    interfaceKit.setOutputState(0, False)
    print "done"

print "Press Enter to quit...."

chr = sys.stdin.read(1)

print "Closing..."

try:
    interfaceKit.closePhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Done."
exit(0)
