CC=gcc

PREFIX=/usr
DEBUG?=y

ifeq ($(DEBUG),y)
DEBUGFLAGS=-g -O0 -D_DEBUG -DDEBUG
else
DEBUGFLAGS=-O2
endif

CFLAGS := $(CFLAGS) -Wall -Wno-strict-aliasing $(DEBUGFLAGS) -fpic -D_LINUX -I. -I../phidget21 -I../phidget21/utils

webservice21cs := eventhandlers.c \
	phidgetinterface.c \
	PhidgetWebservice21.c \
	stdafx.c \
	../phidget21/pdict.c \
	../phidget21/pdictserver.c \
	../phidget21/cphidgetconstants.c \
        ../phidget21/cphidgetlist.c \
        ../phidget21/md5.c \
	../phidget21/utils/plist.c \
	../phidget21/utils/ptree.c \
	../phidget21/utils/utils.c \
	zeroconflinux.c

webservice21objs := $(webservice21cs:.c=.o)

all: phidget21internal.h webservice21

webservice21: $(webservice21objs)
	$(CC) -o phidgetwebservice21 $(webservice21objs) -lphidget21 -lm -ldl

phidget21internal.h:
	echo "#ifndef PHIDGET_H" > $@
	echo "#define PHIDGET_H" >> $@
	echo "#ifdef __cplusplus" >> $@
	echo "extern \"C\" {" >> $@
	echo "#endif" >> $@
	gcc -g -E -P -D_LINUX ../phidget21/phidget21int.h | sed -e "s/extern \"C\" //g" >> $@
	cat ../phidget21/cphidgetconstantsinternal.h >> $@
	cat ../phidget21/cphidgetconstants.h >> $@
	echo "#ifdef __cplusplus" >> $@
	echo "}" >> $@
	echo "#endif" >> $@
	echo "#endif" >> $@

clean: 
	rm *.o phidgetwebservice21 phidget21internal.h

