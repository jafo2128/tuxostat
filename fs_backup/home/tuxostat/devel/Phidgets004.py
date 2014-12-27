# Phidgets004.py
# Python class for controlling the Phidgets InterfaceKit 0/0/4 USB relays.

# Copyright 2008 Jason Antman. <jason@jasonantman.com>.
# Licensed under Version 3 or later of the GNU LGPL.
#  Time-stamp: "2008-06-06 14:36:25 tuxostat"
#  $Id: Phidgets004.py,v 1.1.1.1 2008/06/10 13:50:31 jantman Exp $

class Phidgets004:
    """
    This class controls the Phidgets InterfaceKit 0/0/4 USB relay.
    """
    myDevice = "" # the tty for the relays

    def getZoneTemp(self, OWFSid):
        """
        Reads the temperature file for the specified zone and returns the temperature in Celcius.
        """
        fBase = "/1wire/"
        fEnd = "/temperature"
        fName = fBase+OWFSid+fEnd
        f = open(fName)
        temp = f.read()
        return temp.strip()

    def handleZone(self, zID, OWFSid):
        """
        Handles dealing with the data for each individual zone.
        @param zID - the integer ZoneID (position in the zones list)
        @param OWFSid - the OWFS ID / directory name
        """
        temp = self.getZoneTemp(OWFSid)
        tempC = float(temp)
        tempF = ( 1.8 * tempC ) + 32.0

        # now tempC holds the temp in Celcius and tempF holds the temp in Fahrenheit

        # add to zoneTempsF dict
        self.zoneTempsF[zID] = tempF

        # write out the temp file
        self.writeZoneFile(zID, tempF)

        # TODO: log to MySQL here.

    def writeZoneFile(self, zID, tempF):
        """
        Writes the temp (F) to a text file.
        """
        fName = self.tempFileDir+"zone_"+str(zID)
        f = open(fName, 'w')
        f.write(str(tempF))
        f.close()

    def writeZoneTemps(self):
        """
        Pickle the zoneTempsF file.
        """
        fName = self.tempFileDir+"tempsF.pkl"
        f2 = file(fName, 'w')
        pickle.dump(self.zoneTempsF, f2, False)
        f2.close()

    def main(self):
        ZONES = tosZones() # the zones
        zList = ZONES.GetZoneIDs() # a list of the ZoneIDs to OWFS IDs/dir names

        c = 0 # count

        # iterate through the list
        for OWFSid in zList:
            self.handleZone(c, OWFSid)
            c = c + 1

        self.writeZoneTemps()

if __name__ == '__main__':
    myclass = tosOWFShandler()
    myclass.main()
