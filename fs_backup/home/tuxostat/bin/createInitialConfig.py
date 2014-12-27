#!/usr/bin/env python

# createInitialConfig.py
#  TuxOstat initial config generator
# TuxOstat - Linux-based thermostat <http://www.jasonantman.com>
# Copyright 2008 Jason Antman. <jason@jasonantman.com>.
# Licensed under Version 3 or later of the GNU GPL.
#  Time-stamp: "2008-05-31 18:42:55 jantman"
#  $Id: createInitialConfig.py,v 1.1.1.2 2008/06/10 13:50:31 jantman Exp $

# this is the data array that holds the schedule tasks. each item is a dictionary containing elements [Name, Day, Hour, Minute, Temp, TempMode, ZoneMode, Zone, HVACmode]
# Name - a name to identify this task (doesn't need to be unique)
# Day - the day number in the week - Monday is 0, Sunday is 6
# Hour - the hour of the start time, in 24-hour format
# Minute - the minute of the start time
# Temp - the target temperature
# TempMode - system mode -  0 (default) 1 (Cool Only) 2 (Heat Only) or 3 (Either)
# ZoneMode - how to calculate current temp - 0 (Mean), 1 (Median), 2 (Weighted Mode), 3 (Outliar), 4 (Zone - only worry about specified zones)
# Zone - which thermometer zone to use in Zone mode (dictionary)
# HVACmode - 0 to start HVAC at specified time, 1 to reach specified temperature by specified time

SCHEDULE = []
SCHEDULE.append({'Name': "BackFromHome", 'Day': 0, 'Time': '15:00', 'Temp': 72, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "ToClass", 'Day': 0, 'Time': '17:35', 'Temp': 78, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromClass", 'Day': 0, 'Time': '21:40', 'Temp': 74, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "ToSleep", 'Day': 1, 'Time': '02:00', 'Temp': 75, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromHome", 'Day': 1, 'Time': '08:20', 'Temp': 73, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "ToWork", 'Day': 1, 'Time': '08:45', 'Temp': 80, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromWork", 'Day': 1, 'Time': '17:00', 'Temp': 76, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})


SCHEDULE.append({'Name': "ToClass", 'Day': 1, 'Time': '17:35', 'Temp': 78, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromClass", 'Day': 1, 'Time': '21:40', 'Temp': 74, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "ToSleep", 'Day': 2, 'Time': '02:00', 'Temp': 75, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromHome", 'Day': 2, 'Time': '08:20', 'Temp': 73, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "ToWork", 'Day': 2, 'Time': '08:45', 'Temp': 80, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromWork", 'Day': 2, 'Time': '17:00', 'Temp': 76, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})


SCHEDULE.append({'Name': "ToClass", 'Day': 2, 'Time': '17:35', 'Temp': 78, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromClass", 'Day': 2, 'Time': '21:40', 'Temp': 74, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "ToSleep", 'Day': 3, 'Time': '02:00', 'Temp': 75, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromHome", 'Day': 3, 'Time': '08:20', 'Temp': 73, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "ToWork", 'Day': 3, 'Time': '08:45', 'Temp': 80, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromWork", 'Day': 3, 'Time': '17:00', 'Temp': 76, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})

SCHEDULE.append({'Name': "ToClass", 'Day': 3, 'Time': '17:35', 'Temp': 78, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromClass", 'Day': 3, 'Time': '21:40', 'Temp': 74, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "ToSleep", 'Day': 4, 'Time': '02:00', 'Temp': 75, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromHome", 'Day': 4, 'Time': '08:20', 'Temp': 73, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "ToWork", 'Day': 4, 'Time': '08:45', 'Temp': 80, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromWork", 'Day': 4, 'Time': '17:00', 'Temp': 76, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})

SCHEDULE.append({'Name': "ToClass", 'Day': 4, 'Time': '17:35', 'Temp': 78, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromClass", 'Day': 4, 'Time': '21:40', 'Temp': 74, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "ToSleep", 'Day': 5, 'Time': '02:00', 'Temp': 75, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "BackFromHome", 'Day': 5, 'Time': '08:20', 'Temp': 73, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
SCHEDULE.append({'Name': "ToWork", 'Day': 5, 'Time': '08:45', 'Temp': 80, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})
#SCHEDULE.append({'Name': "BackFromWork", 'Day': 5, 'Time': '17:00', 'Temp': 76, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})

SCHEDULE.append({'Name': "GoneForWeekend", 'Day': 5, 'Time': '18:00', 'Temp': 84, 'TempMode': 1, 'ZoneMode': 0, 'Zone': None, 'HVACmode': 1})

import cPickle as pickle

f1 = file('/etc/tuxostat/schedule.pkl', 'wb')
pickle.dump(SCHEDULE, f1, False)
# TODO: maybe we should use an ASCII format instead?
f1.close()
