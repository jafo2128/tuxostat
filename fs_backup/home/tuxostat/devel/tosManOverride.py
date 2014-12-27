# tosManOverride
# tuxOstat manual overrides
# TuxOstat - Linux-based thermostat <http://www.jasonantman.com>
# Copyright 2008 Jason Antman. <jason@jasonantman.com>.
# Licensed under Version 3 or later of the GNU GPL.
#  Time-stamp: "2008-06-08 00:08:16 tuxostat"
#  $Id: tosManOverride.py,v 1.1.1.1 2008/06/10 13:50:31 jantman Exp $

import cPickle as pickle
import shutil
import time
import os.path

class tosManOverride():
    """
    This class handles all actions related to manual overrides - creating them, reading them, removing them, etc.
    """

    OVERRIDE_FILE = "/var/local/tuxostat/man_override.pkl"
    OVERRIDE = None

    def __init__(self):
        """
        Instantiate the zones object, and load in the zones.
        """
	self.LoadZones()

    def haveOverride(self):
        """
        Returns boolean of whether there is an override set (and active CURRENTLY).
        """
        # if file doesn't exist, return False
        # else parse file, if time.time() is between start and end, then return true

        # have a getOverride() method that returns the whole dict. the dict should have everything that a schedule item has?


    def LoadZones(self):
	"""
	Read-in (unpickle) the zones. This should be called internally only.
	"""
	if os.path.exists(self.ZONE_FILE):
	        f2 = file(self.ZONE_FILE, 'rb')
		self.ZONES = pickle.load(f2)
		f2.close()
	else:
		self.ZONES = []

    def GetZones(self):
        """
        This returns the entire zones, which is a list of dictionaries.
        """
        return self.ZONES

    def GetZone(zoneIndex):
        """
        This returns a list of the given zone index only (i.e. just one zone).
        """
        return self.ZONES[zoneIndex]

    def AddZoneAtEnd(self, zone):
        """
        Add a zone to the end of the list. The order of the zones is somewhat important and should be preserved.
        """
        self.ZONES.append(zone)

    def WriteZones(self):
        """
        Write out (pickle) the zones. First, it moves the old file to /etc/tuxostat/zones_{current_integer_timestamp}.pkl. Returns the filename that the old zones file was moved to.
        """
        if os.path.exists(self.ZONE_FILE):
            newname = '/etc/tuxostat/zones_'+str(int(time.time()))+'.pkl'
            shutil.move(self.ZONE_FILE, newname)
        else:
            newname = "NO ORIGINAL FILE PRESENT"
        f2 = file(self.ZONE_FILE, 'wb')
        pickle.dump(self.ZONES, f2, False)
        f2.close()
	return newname

    def GetNumZones(self):
	"""
	Returns the integer number of zones.
	"""
	return len(self.ZONES)

    def RemoveZone(self, index):
	"""
	Removes the zone at the specified index. In actuality, it doesn't really remove it, but just marks it as Enabled = False.
	"""
        temp = self.ZONES[index]
        temp["Enabled"] = False
        self.zones[index] = temp
	return True

    def ReplaceZone(self, index, zone):
	"""
	Replaces the current zone at index with a new zone dictionary "zone"
	"""
	if index < 0 or index > len(self.ZONES):
	   return False
	self.ZONES[index] = zone
	return True

    def GetZoneIDs(self):
        """
        returns a list of all zone (OWFS) ids in order of ZoneID (ZONES index).
        """

        myList = []

        for z in self.ZONES:
            myList.append(z["ID"])

        return myList

