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
from Phidgets.Devices.MotorControl import *
#import methods for sleeping thread
from time import sleep

#Create an motorcontrol object
motorControl = MotorControl()

#Information Display Function
def displayDeviceInfo():
    print "|------------|----------------------------------|--------------|------------|"
    print "|- Attached -|-              Type              -|- Serial No. -|-  Version -|"
    print "|------------|----------------------------------|--------------|------------|"
    print "|- %8s -|- %30s -|- %10d -|- %8d -|" % (motorControl.isAttached(), motorControl.getDeviceType(), motorControl.getSerialNum(), motorControl.getDeviceVersion())
    print "|------------|----------------------------------|--------------|------------|"
    return 0

#Event Handler Callback Functions
def motorControlAttached(e):
    attached = e.device
    print "MotorControl %i Attached!" % (attached.getSerialNum())
    return 0

def motorControlDetached(e):
    detached = e.device
    print "MotorControl %i Detached!" % (detached.getSerialNum())
    return 0

def motorControlError(e):
    print "Phidget Error %i: %s" % (e.eCode, e.description)
    return 0

def motorControlCurrentChanged(e):
    print "Motor %i Current Draw: %f" % (e.index, e.current)
    return 0

def motorControlInputChanged(e):
    print "Input %i State: %s" % (e.index, e.state)
    return 0

def motorControlVelocityChanged(e):
    print "Motor %i Current Velocity: %f" % (e.index, e.velocity)
    return 0

#Main Program Code
try:
    motorControl.setOnAttachHandler(motorControlAttached)
    motorControl.setOnDetachHandler(motorControlDetached)
    motorControl.setOnErrorhandler(motorControlError)
    motorControl.setOnCurrentChangeHandler(motorControlCurrentChanged)
    motorControl.setOnInputChangeHandler(motorControlInputChanged)
    motorControl.setOnVelocityChangeHandler(motorControlVelocityChanged)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Opening phidget object...."

try:
    motorControl.openPhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Waiting for attach...."

try:
    motorControl.waitForAttach(10000)
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    try:
        motorControl.closePhidget()
    except PhidgetException, e:
        print "Phidget Exception %i: %s" % (e.code, e.message)
        print "Exiting...."
        exit(1)
    print "Exiting...."
    exit(1)
else:
    displayDeviceInfo()

#Control the motor a bit.
print "Will now simulate motor operation...."
print "Step 1: increase acceleration to 50, set target speed at 100"
try:
    motorControl.setAcceleration(1, 50.00)
    motorControl.setVelocity(1, 100.00)
    sleep(5) #sleep for 5 seconds
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)

print "Step 2: Set acceleration to 100, decrease target speed to 75"
try:
    motorControl.setAcceleration(1, 100.00)
    motorControl.setVelocity(1, 75.00)
    sleep(5) #sleep for 5 seconds
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)

print "Step 3: Stop the motor by decreasing speed to 0 at 50 acceleration"
try:
    motorControl.setAcceleration(1, 50.00)
    motorControl.setVelocity(1, 0.00)
    sleep(5) #sleep for 5 seconds
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
else:
    try:
        motorControl.setAcceleration(1, 1.00)
    except PhidgetException, e:
        print "Phidget Exception %i: %s" % (e.code, e.message)

print "Press Enter to quit...."

chr = sys.stdin.read(1)

print "Closing..."

try:
    motorControl.closePhidget()
except PhidgetException, e:
    print "Phidget Exception %i: %s" % (e.code, e.message)
    print "Exiting...."
    exit(1)

print "Done."
exit(0)