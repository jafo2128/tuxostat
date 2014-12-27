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
from Phidgets.Devices.PHSensor import *

#Create an PHSensor object
phSensor = PHSensor()

#Information Display Function
def displayDeviceInfo():
    print "|------------|----------------------------------|--------------|------------|"
    print "|- Attached -|-              Type              -|- Serial No. -|-  Version -|"
    print "|------------|----------------------------------|--------------|------------|"
    print "|- %8s -|- %30s -|- %10d -|- %8d -|" % (phSensor.isAttached(), phSensor.getDeviceType(), phSensor.getSerialNum(), phSensor.getDeviceVersion())
    print "|------------|----------------------------------|--------------|------------|"
    print "PH Sensitivity: %f" % (phSensor.getPHChangeTrigger())
    print "Current Potential: %f" % (phSensor.getPotential())
    return 0

#Event Handler Callback Functions
def phSensorAttached(e):
    attached = e.device
    print "PHSensor %i Attached!" % (attached.getSerialNum())
    return 0

def phSensorDetached(e):
    detached = e.device
    print "PHSensor %i Detached!" % (detached.getSerialNum())
    return 0

def phSensorError(e):
    print "Phidget Error %i: %s" % (e.eCode, e.description)
    return 0

def phSensorPHChanged(e):
    potential = phSensor.getPotential()
    print "PH: %f -- Potential: %f" % (e.PH, potential)
    return 0

#Main Program Code
try:
    phSensor.setOnAttachHandler(phSensorAttached)
    phSensor.setOnDetachHandler(phSensorDetached)
    phSensor.setOnErrorhandler(phSensorError)
    phSensor.setOnPHChangeHandler(phSensorPHChanged)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Opening phidget object...."

try:
    phSensor.openPhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Waiting for attach...."

try:
    phSensor.waitForAttach(10000)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    try:
        phSensor.closePhidget()
    except PhidgetException, e:
        print "Phidget Exception %i: %s" % (e.code, e.message)
        print "Exiting...."
        exit(1)
    print "Exiting...."
    exit(1)
else:
    displayDeviceInfo()

print "Increasing sensitivity to 10.00"
phSensor.setPHChangeTrigger(10.00)

print "Press Enter to quit...."

chr = sys.stdin.read(1)

print "Closing..."

try:
    phSensor.closePhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Done."
exit(0)