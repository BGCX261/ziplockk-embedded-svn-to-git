CFLAGS=-Wall -g `pkg-config --cflags MagickWand`
LDFLAGS=-Wall -g `pkg-config --libs MagickWand`


all: compositor

compositor: compositor.o
	g++ compositor.o ${LDFLAGS} -o compositor

compositor.o: compositor.c
	g++ -c compositor.c ${CFLAGS} -o compositor.o

clean :
	rm -f *.o compositor
