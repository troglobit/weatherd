
#CPPFLAGS = -W -Wall -Wextra
EXEC = sim web sensor
OBJS = db.o strlcpy.o
LDLIBS = -lsqlite3

SENSOROBJS = bme280.o tsl2561.o

all: $(EXEC)

web: web.o $(OBJS)

sim: sim.o $(OBJS)

sensor: sensor.o $(SENSOROBJS)

clean:
	$(RM) $(EXEC) *.o

distclean: clean
	$(RM) *~

