tuxostat
========

[![Project Status: Abandoned - Initial development has started, but there has not yet been a stable, usable release; the project has been abandoned and the author(s) do not intend on continuing development.](http://www.repostatus.org/badges/0.1.0/abandoned.svg)](http://www.repostatus.org/#abandoned)

abandoned linux/python thermostat project

This was a project of mine [quite a few years ago](http://blog.jasonantman.com/2008/06/new-project/) to write a thermostat in Python. It was developed for my apartment in college, which I shared with three roommates; the idea was to determine the union of all of our class and work schedules, and program the thermostat to only heat/cool the apartment when someone was there. This was before Linux-capable embedded systems were easy to obtain, so it ran on an old desktop sitting on the floor under where the original thermostat was. The project "worked" - we used it for a month or so - but had only a command-line interface available over SSH, so we ended up going back to the "normal" thermostat. I never took this further, but some of the code here might be useful.

I've discarded all history when I imported this to Git; I just imported a checkout of the CVS repository. What's here is actually a partial backup of the filesystem that ran this thing. The important parts are under:

* [/usr/lib/python2.4/site-packages/tuxostat](https://github.com/jantman/tuxostat/blob/master/fs_backup/usr/lib/python2.4/site-packages/tuxostat)
* [/home/tuxostat/bin](https://github.com/jantman/tuxostat/blob/master/fs_backup/home/tuxostat/bin)

It uses the [Phidgets Python libraries](http://www.phidgets.com/docs/Language_-_Python) (I'm sure ancient versions) to control the Phidgets USB relays, and uses something that [I wrote](https://github.com/jantman/tuxostat/blob/master/fs_backup/usr/lib/python2.4/site-packages/tuxostat/crystalfontz635usb.py) to control a [CrystalFontz CF635 USB LCD display](https://www.crystalfontz.com/product/XES635BKTMFKU).

Blog posts relating to this:

* [Project announcement](http://blog.jasonantman.com/2008/06/new-project/) June 2008
* [Project update](http://blog.jasonantman.com/2008/06/update-2/) June 2008
* [Project Status when I left off](http://blog.jasonantman.com/2008/07/massive-updates/) July 2008
