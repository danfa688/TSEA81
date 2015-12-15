CFLAGS=-O2 -g -Wall
LIBS=-lrt -lpthread
CFILES=main.c
lift_pthreads: $(CFILES)
	gcc $(CFLAGS) $(CFILES) -o mutex $(LIBS)

clean:
	rm -f mutex *~ *.o
