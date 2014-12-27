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
from Phidgets.Devices.TemperatureSensor import *

#Create an temperaturesensor object
temperatureSensor = TemperatureSensor()

#Information Display Function
def DisplayDeviceInfo():
    inputCount = temperatureSensor.getTemperatureInputCount()
    print "|------------|----------------------------------|--------------|------------|"
    print "|- Attached -|-              Type              -|- Serial No. -|-  Version -|"
    print "|------------|----------------------------------|--------------|------------|"
    print "|- %8s -|- %30s -|- %10d -|- %8d -|" % (temperatureSensor.isAttached(), temperatureSensor.getDeviceType(), temperatureSensor.getSerialNum(), temperatureSensor.getDeviceVersion())
    print "|------------|----------------------------------|--------------|------------|"
    print "Number of Temperature Inputs: %i" % (inputCount)
    for i in range(inputCount):
        print "Input %i Sensitivity: %f" % (i, temperatureSensor.getTemperatureChangeTrigger(i))
    return 0

#Event Handler Callback Functions
def TemperatureSensorAttached(e):
    attached = e.device
    print "TemperatureSensor %i Attached!" % (attached.getSerialNum())
    return 0

def TemperatureSensorDetached(e):
    detached = e.device
    print "TemperatureSensor %i Detached!" % (detached.getSerialNum())
    return 0

def TemperatureSensorError(e):
    print "Phidget Error %i: %s" % (e.eCode, e.description)
    return 0

def TemperatureSensorTemperatureChanged(e):
    try:
        ambient = temperatureSensor.getAmbientTemperature()
    except PhidgetException, e:
        print "Phidget Exception %i: %s" % (e.code, e.message)
        ambient = 0.00
    
    print "Ambient Temp: %f -- Thermocouple %i temperature: %f -- Potential: %f" % (ambient, e.index, e.temperature, e.potential)
    
    return 0

#Main Program Code
try:
    temperatureSensor.setOnAttachHandler(TemperatureSensorAttached)
    temperatureSensor.setOnDetachHandler(TemperatureSensorDetached)
    temperatureSensor.setOnErrorhandler(TemperatureSensorError)
    temperatureSensor.setOnTemperatureChangeHandler(TemperatureSensorTemperatureChanged)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Opening phidget object...."

try:
    temperatureSensor.openPhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Waiting for attach...."

try:
    temperatureSensor.waitForAttach(10000)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    try:
        temperatureSensor.closePhidget()
    except PhidgetException, e:
        print "Phidget Exception %i: %s" % (e.code, e.message)
        print "Exiting...."
        exit(1)
    print "Exiting...."
    exit(1)
else:
    DisplayDeviceInfo()

print "Setting sensitivity of the thermocouple to 2.00...."
temperatureSensor.setTemperatureChangeTrigger(0, 2.00)

print "Press Enter to quit...."

chr = sys.stdin.read(1)

print "Closing..."

try:
    temperatureSensor.closePhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Done."
exit(0)