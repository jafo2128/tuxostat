#Makefile for Crystalfontz ./test635
#Please read the README for details on using the 'test635' utility and 'make' options

CC = g++
DEBUGFLAG = -g
LIBS = -lstdc++
DEBUGFLAG = -g
CFLAGS = $(DEBUGFLAG)
LDFLAGS = $(DEBUGFLAG)

SRCS = 635_packet.c show_packet.c serial.c test635.c
OBJS = $(SRCS:.c=.o)
TARGET = test635

all:	$(TARGET)

$(TARGET):	$(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

distribution:	clean
	tar -C .. -cvf ../635_linux.tar 635_linux
