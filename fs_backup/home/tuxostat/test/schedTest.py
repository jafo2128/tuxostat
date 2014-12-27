from tuxostat.tosSched import tosSched
import time

mySched = tosSched()

SCHED = mySched.GetSchedule()

count = 0
for i in SCHED:
    print str(count)+"\t"+str(i)
    count = count + 1


print "==================================================================="


def GetCurrentTaskID():
    """
    Returns the full dict for the next task. This ASSUMES that the whole schedule is in chronological order.
    """

    nowtime = time.localtime() # struct_time for NOW
    nowday = nowtime[6]
    nowhr = nowtime[3]
    nowmin = nowtime[4]
    nowmins = nowmin + (nowhr * 60)

    print "nowday="+str(nowday)+" nowhr="+str(nowhr)+" nowmin="+str(nowmin)+" nowmins="+str(nowmins)

    # iterate through the schedule
    count = 0
    for t in SCHED:
        print "for t in SCHED count = "+str(count)

        tday = t["Day"]
        tmin = int(t["Time"][t["Time"].find(":")+1:])
        thr = int(t["Time"][:t["Time"].find(":")])
        tmins = tmin + (thr * 60)
        print "tday="+str(tday)+" thr="+str(thr)+" tmin="+str(tmin)+" tmins="+str(tmins)
        
        
        if tday > nowday:
            # we just went into tomorrow
            print "tday > nowday count="+str(count)+" RETURNING "+str(count-1)
            return (count-1)
        elif tday == nowday:
            # same day of the week, look at the hours:
            print "tday == nowday"
            if tmins > nowmins:
                print "thmins > nowmins count="+str(count)+" returning "+str(count-1)
                return (count-1)
        # increment count
        count = count + 1

    # if we got here, we're at the last schedule item. return that.
    return len(count)


print "Current Task: "+str(GetCurrentTaskID())

