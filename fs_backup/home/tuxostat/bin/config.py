#!/usr/bin/env python

# config.py
#  TuxOstat configuration program
# TuxOstat - Linux-based thermostat <http://www.jasonantman.com>
# Copyright 2008 Jason Antman. <jason@jasonantman.com>.
# Licensed under Version 3 or later of the GNU GPL.
#  Time-stamp: "2008-06-02 14:10:51 jantman"
#  $Id: config.py,v 1.4 2008/06/02 18:09:25 jantman Exp $


#
# IMPORTS
#
import os, sys

from tuxOstat_config import *

#
# GLOBAL VARIABLES
#


# START BANNER - this is the program banner. it goes on for a while...
# TODO: put the banner in here rather than using a shell exec
# END BANNER

#
# FUNCTION DEFINITIONS
#

def showEnv():
    """
    This function shows the current status of the environmental sensors.
    """
    # TODO: this function
    print "environment"

def showSched():
    """
    This function shows the current schedule values. It should be formatted for a 80x24 terminal screen.
    """
    global SCHEDULE

    mySched = SCHEDULE.GetSchedule()
    print "--------------------------------------------------------------------------------"
    print "ID   Day   Time   Z   ZM     Temp   TM     HVACm   Name"
    print "--------------------------------------------------------------------------------"

    # print each line in the schedule
    count = 0
    for i in mySched:
        # format hour and minute
        hr = str(i['Hour'])
        min = str(i['Minute'])
        if len(hr) == 1:
            hr = "0"+hr
        if len(min) == 1:
            min = "0"+min
        # format Zone if none
        if i['Zone'] is None:
            i['Zone'] = "-"
        # formatted count string
        cs = str(count)
        if len(cs) == 1:
            cs = cs+" "
        # the full listing
        s = cs+"   "+SCHEDULE.days[i['Day']]+"    "+hr+":"+min+"  "+str(i['Zone'])+"   "+SCHEDULE.zoneModes[i['ZoneMode']]+"   "+str(i['Temp'])+"     "+SCHEDULE.tempModes[i['TempMode']]+"   "+SCHEDULE.HVACmodes[i['HVACmode']]+"    "+i['Name']
        print s
        count = count + 1

def getTaskSummary(taskID):
    """
    Returns a formatted string, a detailed summary of the specified taskID.
    """
    global SCHEDULE

    mySched = SCHEDULE.GetSchedule()
    if len(mySched) < (taskID - 1):
        return None

    myTask = mySched[taskID]

    # format the time
    hr = str(myTask['Hour'])
    min = str(myTask['Minute'])
    if len(hr) == 1:
        hr = "0"+hr
    if len(min) == 1:
        min = "0"+min

    s = "Name: "+myTask['Name']+"\n"
    s = s+"Day: "+SCHEDULE.days[myTask['Day']]+"\t"+"Start Time: "+hr+":"+min+"\n"
    s = s+"Zone: "+"---"+"\t"+"Zone Mode: "+SCHEDULE.zoneModes[myTask['ZoneMode']]+"\n" # TODO: get zone name
    s = s+"Temp: "+str(myTask['Temp'])+"\t"+"Temp Mode: "+SCHEDULE.tempModes[myTask['TempMode']]+"\n"
    s = s+"HVAC Mode: "+SCHEDULE.HVACmodes[myTask['HVACmode']]
    return s


def addTaskBefore():
    """
    Adds a task to the schedule before beforeID.
    """
    resp = raw_input("Task ID to add new task before: ")
    print "adding task before "+resp # DEBUG

def addTaskAtEnd():
    """
    Adds a new task at the end of the current schedule.
    """
    print "add at end"

def removeTask():
    """
    Removes a specified task.
    """
    resp = raw_input("Task ID to remove: ")
    # TODO: print a summary of the task and ask for confirmation.
    print "removing task "+resp

def editTask():
    """
    Edits a specified task.
    """
    
    resp = raw_input("Task ID to edit: ")
    # TODO: show a summary of the task and then give an editing menu than continues until saved or canceled

    print "SUMMARY OF TASK ID "+resp+":"
    print getTaskSummary(int(resp))

def editSched():
    """
    This function handles editing the schedule.
    """

    print "1) Add task before given ID."
    print "2) Add task at end."
    print "3) Remove task."
    print "4) Edit task."
    print "5) Cancel."

    resp = raw_input("Selection: ")

    if resp == "1":
        addTaskBefore()
    elif resp == "2":
        addTaskAtEnd()
    elif resp == "3":
        removeTask()
    elif resp == "4":
        editTask()
    elif resp == "5":
        # cancel
        print "Canceled."
        return 1
    else:
        print "Invalid Selection. Canceled."
        return 1

def editConfig():
    """
    Edits the current system configuration.
    """
    # TODO: implement this
    print "call editConfig()"

def showConfig():
    """
    Shows the current system configuration.
    """
    # TODO: implement this
    print "call showConfig()"

def manOverride():
    """
    Adds an immediate manual override.
    """
    # TODO: implement this
    print "manual override"

def schedOverride():
    """
    Schedule an override for a specific time period.
    """
    # TODO: implement this.
    print "sched override"

def restart():
    """
    This function will restart the system.
    """
    clear()
    print "\n\n\n\n\n\n"
    print "WARNING: you have selected to RESTART the system."
    print "To restart, type PLEASE RESTART. TO cancel, type anything else."
    resp = raw_input("Are you sure? I mean really, really, really sure? ")
    if resp == "PLEASE RESTART":
        print "TODO: RESTART SYSTEM."
        # TODO: will this still work if we exit here?
        #TODO: implement this
        # RESTART HERE
        sys.exit()
    else:
        clear()
        return 0

def showZones():
    """
    This shows the current zone IDs, names, and ID strings (1-wire IDs)
    """
    #TODO: implement this
    print "show zones"

def editZones():
    """
    This provides an interface for editing zones.
    """
    #TODO: implement this
    print "edit zones"

def showHelp():
    """
    This function prints names and descriptions of all available commands.
    """

    print ""
    print "HELP - show this help"
    print "HELP DISPLAY - show help about the physical display"
    print "EXIT - exit/logout"
    print "SHOW SCHED - show the current schedule"
    print "EDIT SCHED - edit the current schedule"
    print "SCHED OVERRIDE - add a specific day/time schedule override"
    print "MAN OVERRIDE - override schedule manually - NOW"
    print "SHOW CONFIG - show current system config"
    print "EDIT CONFIG - edit the configuration"
    print "SHOW ENV - show current environmental sensor data"
    print "SHOW ZONES - show names, IDs, and numbers of all configured 1-wire zones"
    print "EDIT ZONES - edit names and IDs of all configured zones"
    print "RESTART - restart the system"
    print ""

def helpDisplay():
    """
    This function shows help about the physical display.
    """
    # TODO: implement
    os.system("cat display.txt") # TODO: fix character encoding and store locally

def clear():
    """
    Just makes an os.system call to 'clear'.
    """
    os.system("clear")

def showBanner():
    """
    Prints the program banner.
    """
    clear()
    os.system("cat banner.txt") # TODO: fix character encoding and store locally
    print "For help, enter HELP at the prompt."
    print " "

def mainLoop():
    resp = raw_input("TuxOstat # ")
    resp = resp.upper()
    if resp == "HELP":
        showHelp()
        return 1
    elif resp == "HELP DISPLAY":
        helpDisplay()
        return 1
    elif resp == "EXIT":
        sys.exit(0)
    elif resp == "SHOW CONFIG":
        showConfig()
        return 1
    elif resp == "SHOW ENV":
        showEnv()
        return 1
    elif resp == "EDIT CONFIG":
        editConfig()
        return 1
    elif resp == "SHOW ZONES":
        showZones()
        return 1
    elif resp == "EDIT ZONES":
        editZones()
        return 1
    elif resp == "SHOW SCHED":
        showSched()
        return 1
    elif resp == "EDIT SCHED":
        editSched()
        return 1
    elif resp == "SCHED OVERRIDE":
        schedOverride()
        return 1
    elif resp == "MAN OVERRIDE":
        manOverride()
        return 1
    elif resp == "RESTART":
        restart()
        return 3
    else:
        print "Invalid command."
        return -1
#
# BEGIN MAIN PROGRAM LOOP
#


SCHEDULE = tuxOstat_config() # instantiate the schedule object

showBanner()
mainResult = -1
while mainResult != 0:
    mainResult = mainLoop()
