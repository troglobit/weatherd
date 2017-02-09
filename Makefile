
#CPPFLAGS = -W -Wall -Wextra
EXEC = sim web sensor weatherd
OBJS = db.o strlcpy.o bme280.o tsl2561.o
LDLIBS = -lsqlite3

all: $(EXEC)

web: web.o $(OBJS)

sim: sim.o $(OBJS)

sensor: sensor.o $(OBJS)

weatherd: weatherd.o  $(OBJS)

clean:
	$(RM) $(EXEC) *.o

distclean: clean
	$(RM) *~

