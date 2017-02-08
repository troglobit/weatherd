

EXEC = sim web
OBJS = db.o strlcpy.o
LDLIBS = -lsqlite3

all: $(EXEC)

web: web.o $(OBJS)

sim: sim.o $(OBJS)

clean:
	$(RM) $(EXEC) *.o

distclean: clean
	$(RM) *~

