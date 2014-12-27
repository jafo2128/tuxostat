#!/usr/bin/env python

# config.py
#  TuxOstat configuration program
# TuxOstat - Linux-based thermostat <http://www.jasonantman.com>
# Copyright 2008 Jason Antman. <jason@jasonantman.com>.
# Licensed under Version 3 or later of the GNU GPL.
#  Time-stamp: "2008-06-01 13:46:36 jantman"
#  $Id: tuxOstat_config.py,v 1.1.1.1 2008/06/01 17:56:34 jantman Exp $

import cPickle as pickle

class tuxOstat_config():
    """
    This class handles all reading and writing of the configuration files and schedule.
    """

    SCHEDULE_FILE = '../../../etc/tuxostat/schedule.pkl'
    SCHEDULE = None

    # non-private variables
    days = {0: "M ", 1: "Tu", 2: "W ", 3: "Th", 4: "F ", 5: "Sa", 6: "Su"}
    tempModes = {0: "DEF ", 1: "COOL", 2: "HEAT", 3: "EITH"} # string length 4
    zoneModes = {0: "Mean", 1: "MED ", 2: "Wmed", 3: "OUTL", 4: "ZONE"} # string length 4
    HVACmodes = {0: "Time", 1: "Temp"}

    def __init__(self):
        """
        Instantiate the config object, and load in the config and schedule.
        """
        f2 = file(self.SCHEDULE_FILE, 'rb')
        self.SCHEDULE = pickle.load(f2)
        f2.close()

    def GetSchedule(self):
        """
        This returns the entire schedule, which is a list of dictionaries.
        """
        return self.SCHEDULE

    def AddTaskBefore(self, task, beforeIndex):
        """
        This adds a new task into the schedule before the specified index. The task should be a dictionary. This function does NOT write out the schedule (pickle it), it just adds to the current data structure, so the user has a chance to review and confirm changes.
        """
        self.SCHEDULE.add(beforeIndex, task)

    def WriteSchedule(self):
        """
        Write out (pickle) the schedule.
        """
        f2 = file(self.SCHEDULE_FILE, 'wb')
        pickle.dump(self.SCHEDULE, f1, False)
        f2.close()
