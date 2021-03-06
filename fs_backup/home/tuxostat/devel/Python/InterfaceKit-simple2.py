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
from Phidgets.Devices.InterfaceKit import *

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

#Event Handler Callback Functions
def inferfaceKitAttached(e):
    attached = e.device
    print "InterfaceKit %i Attached!" % (attached.getSerialNum())
    return 0

def interfaceKitDetached(e):
    detached = e.device
    print "InterfaceKit %i Detached!" % (detached.getSerialNum())
    return 0

def interfaceKitError(e):
    print "Phidget Error %i: %s" % (e.eCode, e.description)
    return 0

def interfaceKitInputChanged(e):
    print "Input %i: %s" % (e.index, e.state)
    return 0

def interfaceKitSensorChanged(e):
    print "Sensor %i: %i" % (e.index, e.value)
    return 0

def interfaceKitOutputChanged(e):
    print "Output %i: %s" % (e.index, e.state)
    return 0

#Main Program Code
try:
    interfaceKit.setOnAttachHandler(inferfaceKitAttached)
    interfaceKit.setOnDetachHandler(interfaceKitDetached)
    interfaceKit.setOnErrorhandler(interfaceKitError)
    interfaceKit.setOnInputChangeHandler(interfaceKitInputChanged)
    interfaceKit.setOnOutputChangeHandler(interfaceKitOutputChanged)
    interfaceKit.setOnSensorChangeHandler(interfaceKitSensorChanged)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

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
else:
    displayDeviceInfo()


print "Output Count: "+str(interfaceKit.getOutputCount())

print "Output 0 is currently in state "+str(interfaceKit.getOutputState(0))
print "Press Enter to toggle output 0"
chr = sys.stdin.read(1)
if interfaceKit.getOutputState(0) is False:
    print "setting output dtate to True"
    interfaceKit.setOutputState(0, True)
    print "done"
else:
    print "setting output dtate to False"
    interfaceKit.setOutputState(0, False)
    print "done"

print "Output 0 is currently in state "+str(interfaceKit.getOutputState(0))
print "Press Enter to toggle output 0"
chr = sys.stdin.read(1)
if interfaceKit.getOutputState(0) is False:
    print "setting output dtate to True"
    interfaceKit.setOutputState(0, True)
    print "done"
else:
    print "setting output dtate to False"
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
