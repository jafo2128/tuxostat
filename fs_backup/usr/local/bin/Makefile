# makefile to update CVS and do anything else needed
# $Id: Makefile,v 1.1.1.1 2008/06/11 17:42:24 jantman Exp $

ifdef LOG
        LOGSTR = $(LOG)
else
        LOGSTR = "just working on things"
endif

cvsupdate:
	cvs import -m "$(LOGSTR)" tuxostat/usr/local/bin jantman trunk