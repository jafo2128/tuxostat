#! /usr/bin/env python
# tuxostat Phidgets 0/0/4 relay test script

from tuxostat.tosRelayControl import tosRelayControl

trc = tosRelayControl()

option = 0
while option != 3:
    print "0) Show relay state"
    print "1) Set Relay ON (True)"
    print "2) Set Relay OFF (False)"
    print "3) EXIT"
    option = raw_input("SELECTION: ")

    if option == "0":
        # show state
        print "0\t"+str(trc.GetOutputState(0))
        print "1\t"+str(trc.GetOutputState(1))
        print "2\t"+str(trc.GetOutputState(2))
        print "3\t"+str(trc.GetOutputState(3))
    elif option == "1":
        # set on
        relay = int(raw_input("Relay Number: "))
        print "Setting output "+str(relay)+" to ON..."
        trc.SetOutputState(relay, True)
        print "Done."
    elif option == "2":
        # set off
        relay = int(raw_input("Relay Number: "))
        print "Setting output "+str(relay)+" to OFF..."
        trc.SetOutputState(relay, False)
        print "Done."
    else:
        break

# close
print "cleaning up..."
trc.close()
print "Exiting."
