# makefile to update CVS and do anything else needed
# $Id: Makefile,v 1.1.1.3 2008/06/11 23:40:20 jantman Exp $

ifdef LOG
        LOGSTR = $(LOG)
else
        LOGSTR = just working on things
endif

cvsupdate:
	cvs import -m "$(LOGSTR)" tuxostat/home/tuxostat jantman trunk
	cd /usr/local/bin && cvs import -m "$(LOGSTR)" tuxostat/usr/local/bin jantman trunk
	cd /usr/lib/python2.4/site-packages/tuxostat/ && cvs import -m "$(LOGSTR)" tuxostat/usr/lib/python2.4/site-packages/tuxostat jantman trunk
	cd /etc/tuxostat && cvs import -m "$(LOGSTR)" tuxostat/etc/tuxostat jantman trunk
	cd /var/local/tuxostat && cvs import -m "$(LOGSTR)" tuxostat/var/local/tuxostat jantman trunk
	cd /var/www/html && cvs import -m "$(LOGSTR)" tuxostat/var/www/html jantman trunk