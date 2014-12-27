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
from Phidgets.Devices.Encoder import *

#Create an accelerometer object
encoder = Encoder()

#Information Display Function
def displayDeviceInfo():
    print "|------------|----------------------------------|--------------|------------|"
    print "|- Attached -|-              Type              -|- Serial No. -|-  Version -|"
    print "|------------|----------------------------------|--------------|------------|"
    print "|- %8s -|- %30s -|- %10d -|- %8d -|" % (encoder.isAttached(), encoder.getDeviceType(), encoder.getSerialNum(), encoder.getDeviceVersion())
    print "|------------|----------------------------------|--------------|------------|"
    return 0

#Event Handler Callback Functions
def encoderAttached(e):
    attached = e.device
    print "Encoder %i Attached!" % (attached.getSerialNum())
    return 0

def encoderDetached(e):
    detached = e.device
    print "Encoder %i Detached!" % (detached.getSerialNum())
    return 0

def encoderError(e):
    print "Phidget Error %i: %s" % (e.eCode, e.description)
    return 0

def encoderInputChange(e):
    print "Input %i: %s" % (e.index, e.state)
    return 0

def encoderPositionChange(e):
    print "Encoder %i -- Change: %i -- Time: %i -- Position: %i" % (e.index, e.positionChange, e.time, encoder.getPosition(e.index))
    return 0

#Main Program Code
try:
    encoder.setOnAttachHandler(encoderAttached)
    encoder.setOnDetachHandler(encoderDetached)
    encoder.setOnErrorhandler(encoderError)
    encoder.setOnInputChangeHandler(encoderInputChange)
    encoder.setOnPositionChangeHandler(encoderPositionChange)
except PhidgetException, e:
    print "Phidget Error %i: %s" % (e.code, e.message)
    exit(1)

print "Opening phidget object...."

try:
    encoder.openPhidget()
except PhidgetException, e:
    print "Phidget Error %i: %s" % (e.code, e.message)
    exit(1)

print "Waiting for attach...."

try:
    encoder.waitForAttach(10000)
except PhidgetException, e:
    print "Phidget Error %i: %s" % (e.code, e.message)
    try:
        encoder.closePhidget()
    except PhidgetException, e:
        print "Phidget Error %i: %s" % (e.code, e.message)
        exit(1)
    exit(1)
else:
    displayDeviceInfo()

print "Press Enter to quit...."

chr = sys.stdin.read(1)

print "Closing..."

try:
    encoder.closePhidget()
except PhidgetException, e:
    print "Phidget Error %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Done."
exit(0)