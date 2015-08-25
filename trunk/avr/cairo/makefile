CFLAGS=-Wall -pedantic -g -I/usr/X11R6/include `pkg-config --cflags cairo`
LDFLAGS=-Wall -g `pkg-config --libs cairo` -L/usr/X11R6/lib -lX11


all: generator

generator: generator.o
	g++ generator.o ${LDFLAGS} -o generator

generator.o: generator.c
	g++ -c generator.c ${CFLAGS} -o generator.o

clean :
	rm -f *.o generator
