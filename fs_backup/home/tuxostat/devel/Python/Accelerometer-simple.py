#!/usr/bin/env python

"""Copyright 2008 Phidgets Inc.
This work is licensed under the Creative Commons Attribution 2.5 Canada License. 
To view a copy of this license, visit http://creativecommons.org/licenses/by/2.5/ca/
"""

__author__ = 'Adam Stelmack'
__version__ = '2.1.4'
__date__ = 'May 02 2008'

#Basic imports
from threading import *
from ctypes import *
import sys
#Phidget specific imports
from Phidgets.PhidgetException import *
from Phidgets.Events.Events import *
from Phidgets.Devices.Accelerometer import *

#Create an accelerometer object
accelerometer = Accelerometer()

#Information Display Function
def DisplayDeviceInfo():
    print "|------------|----------------------------------|--------------|------------|"
    print "|- Attached -|-              Type              -|- Serial No. -|-  Version -|"
    print "|------------|----------------------------------|--------------|------------|"
    print "|- %8s -|- %30s -|- %10d -|- %8d -|" % (accelerometer.isAttached(), accelerometer.getDeviceType(), accelerometer.getSerialNum(), accelerometer.getDeviceVersion())
    print "|------------|----------------------------------|--------------|------------|"
    print "Number of Axes: %i" % (accelerometer.getAxisCount())
    return 0

#Event Handler Callback Functions
def AccelerometerAttached(e):
    attached = e.device
    print "Accelerometer %i Attached!" % (attached.getSerialNum())
    return 0

def AccelerometerDetached(e):
    detached = e.device
    print "Accelerometer %i Detached!" % (detached.getSerialNum())
    return 0

def AccelerometerError(e):
    print "Phidget Error %i: %s" % (e.eCode, e.description)
    return 0

def AccelerometerAccelerationChanged(e):
    print "Axis %i: %6f" % (e.index, e.acceleration)
    return 0

#Main Program Code
try:
    accelerometer.setOnAttachHandler(AccelerometerAttached)
    accelerometer.setOnDetachHandler(AccelerometerDetached)
    accelerometer.setOnErrorhandler(AccelerometerError)
    accelerometer.setOnAccelerationChangeHandler(AccelerometerAccelerationChanged)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Opening phidget object...."

try:
    accelerometer.openPhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Waiting for attach...."

try:
    accelerometer.waitForAttach(10000)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    try:
        accelerometer.closePhidget()
    except PhidgetException, e:
        print "Phidget Exception %i: %s" % (e.code, e.message)
        print "Exiting...."
        exit(1)
    print "Exiting...."
    exit(1)
else:
    try:
        numAxis = accelerometer.getAxisCount()
        accelerometer.setAccelChangeTrigger(0, 0.500)
        accelerometer.setAccelChangeTrigger(1, 0.500)
        if numAxis > 2:
            accelerometer.setAccelChangeTrigger(2, 0.500)
    except PhidgetException, e:
        print "Phidget Exception %i: %s" % (e.code, e.message)
    
    DisplayDeviceInfo()

print "Press Enter to quit...."

chr = sys.stdin.read(1)

print "Closing..."

try:
    accelerometer.closePhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Done."
exit(0)