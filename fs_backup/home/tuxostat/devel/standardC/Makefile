OBJECTS=Accelerometer-simple \
	Encoder-simple \
	InterfaceKit-simple \
	LED-simple \
	Manager-simple \
	MotorControl-simple \
	PHSensor-simple \
	RFID-simple \
	Servo-simple \
	TemperatureSensor-simple \
	TextLCD-simple

CFLAGS=-g -O0 -Wall 
LIBS= -lphidget21 -lm

all: $(OBJECTS)

%: %.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	-rm *.o $(OBJECTS)
