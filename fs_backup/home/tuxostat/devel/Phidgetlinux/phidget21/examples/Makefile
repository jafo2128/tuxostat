OBJECTS=manager ifkit
CFLAGS=-g -O0 -Wall 
LIBS= -lphidget21
LDFLAGS=

all: $(OBJECTS)

manager: manager.o
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

ifkit: ifkit.o
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
	
clean:
	-rm *.o $(OBJECTS)
