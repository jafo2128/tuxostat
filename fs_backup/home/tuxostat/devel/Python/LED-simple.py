#!/usr/bin/env python

"""Copyright 2008 Phidgets Inc.
This work is licensed under the Creative Commons Attribution 2.5 Canada License. 
To view a copy of this license, visit http://creativecommons.org/licenses/by/2.5/ca/
"""

__author__ = 'Adam Stelmack'
__version__ = '2.1.4'
__date__ = 'May 02 2008'

#Basic imports
from ctypes import *
import sys
#Phidget specific imports
from Phidgets.PhidgetException import *
from Phidgets.Events.Events import *
from Phidgets.Devices.LED import *
from time import sleep

#Create an LED object
led = LED()

#Information Display Function
def displayDeviceInfo():
    print "|------------|----------------------------------|--------------|------------|"
    print "|- Attached -|-              Type              -|- Serial No. -|-  Version -|"
    print "|------------|----------------------------------|--------------|------------|"
    print "|- %8s -|- %30s -|- %10d -|- %8d -|" % (led.isAttached(), led.getDeviceType(), led.getSerialNum(), led.getDeviceVersion())
    print "|------------|----------------------------------|--------------|------------|"
    return 0

#Event Handler Callback Functions
def ledAttached(e):
    attached = e.device
    print "LED %i Attached!" % (attached.getSerialNum())
    return 0

def ledDetached(e):
    detached = e.device
    print "LED %i Detached!" % (detached.getSerialNum())
    return 0

def ledError(e):
    print "Phidget Error %i: %s" % (e.eCode, e.description)
    return 0

#Main Program Code
try:
    led.setOnAttachHandler(ledAttached)
    led.setOnDetachHandler(ledDetached)
    led.setOnErrorhandler(ledError)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    exit(1)

print "Opening phidget object..."

try:
    led.openPhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    exit(1)

print "Waiting for attach...."

try:
    led.waitForAttach(10000)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    try:
        led.closePhidget()
    except PhidgetException, e:
        print "Phidget Exception %i: %s" % (e.code, e.message)
        exit(1)
    exit(1)
else:
    displayDeviceInfo()

#This example assumes that there are LED's plugged into locations 0-7

print "Turning on LED's 0 - 9"

for i in range(8):
    if i < 7:
        print i,
    else:
        print i
    led.setDiscreteLED(i, 100)
    sleep(1)

print "Turning off LED's 0 - 9"

for i in range(8):
    if i < 7:
        print i,
    else:
        print i
    led.setDiscreteLED(i, 0)
    sleep(1)

print "Press Enter to quit...."

chr = sys.stdin.read(1)

print "Closing..."

try:
    led.closePhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting..."
    exit(1)

print "Done."
exit(0)