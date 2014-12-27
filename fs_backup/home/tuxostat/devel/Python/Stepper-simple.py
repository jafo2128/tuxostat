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
from time import sleep
#Phidget specific imports
from Phidgets.PhidgetException import *
from Phidgets.Events.Events import *
from Phidgets.Devices.Stepper import *

#Create a stepper object
stepper = Stepper()

#Information Display Function
def DisplayDeviceInfo():
    print "|------------|----------------------------------|--------------|------------|"
    print "|- Attached -|-              Type              -|- Serial No. -|-  Version -|"
    print "|------------|----------------------------------|--------------|------------|"
    print "|- %8s -|- %30s -|- %10d -|- %8d -|" % (stepper.isAttached(), stepper.getDeviceType(), stepper.getSerialNum(), stepper.getDeviceVersion())
    print "|------------|----------------------------------|--------------|------------|"
    print "Number of Motors: %i" % (stepper.getMotorCount())
    return 0

#Event Handler Callback Functions
def StepperAttached(e):
    attached = e.device
    print "Stepper %i Attached!" % (attached.getSerialNum())
    return 0

def StepperDetached(e):
    detached = e.device
    print "Stepper %i Detached!" % (detached.getSerialNum())
    return 0

def StepperError(e):
    print "Phidget Error %i: %s" % (e.eCode, e.description)
    return 0

def StepperCurrentChanged(e):
    print "Motor %i -- Current Draw: %6f" % (e.index, e.current)
    return 0

def StepperInputChanged(e):
    print "Input %i -- State: %s" % (e.index, e.state)
    return 0

def StepperPositionChanged(e):
    print "Motor %i -- Position: %f" % (e.index, e.position)
    return 0

def StepperVelocityChanged(e):
    print "Motor %i -- Velocity: %f" % (e.index, e.velocity)
    return 0

#Main Program Code
try:
    stepper.setOnAttachHandler(StepperAttached)
    stepper.setOnDetachHandler(StepperDetached)
    stepper.setOnErrorhandler(StepperError)
    stepper.setOnCurrentChangeHandler(StepperCurrentChanged)
    stepper.setOnInputChangeHandler(StepperInputChanged)
    stepper.setOnPositionChangeHandler(StepperPositionChanged)
    stepper.setOnVelocityChangeHandler(StepperVelocityChanged)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Opening phidget object...."

try:
    stepper.openPhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Waiting for attach...."

try:
    stepper.waitForAttach(10000)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    try:
        stepper.closePhidget()
    except PhidgetException, e:
        print "Phidget Exception %i: %s" % (e.code, e.message)
        print "Exiting...."
        exit(1)
    print "Exiting...."
    exit(1)
else:
    DisplayDeviceInfo()

try:
    print "Set the current position as start position..."
    stepper.setCurrentPosition(0, 0)
    sleep(1)
    
    print "Set the motor as engaged..."
    stepper.setEngaged(0, True)
    sleep(1)
    
    print "Setting Velocity and Acceleration to Maximum for this example..."
    print "The motor will run until it reaches the set goal position..."
    
    accelerationMax = stepper.getAccelerationMax(0)
    velocityMax = stepper.getVelocityMax(0)
    stepper.setAcceleration(0, accelerationMax)
    stepper.setVelocityLimit(0, velocityMax)
    sleep(2)
    
    print "Will now move to position 2000..."
    stepper.setTargetPosition(0, 2000)
    while stepper.getCurrentPosition(0) != 2000:
        pass
    
    sleep(2)
    
    print "Will now move back to positon 0..."
    stepper.setTargetPosition(0, 0)
    while stepper.getCurrentPosition(0) != 0:
        pass
    
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Press Enter to quit...."

chr = sys.stdin.read(1)

print "Closing..."

try:
    stepper.closePhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Done."
exit(0)