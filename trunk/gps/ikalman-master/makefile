APP = gps_test
FILES = gps kalman  gps_test  matrix 
CFLAGS = -std=c99 -Wno-implicit-function-declaration
OFLAGS = -lm
CFILES = $(foreach file,$(FILES),$(file).cc)
OFILES = $(foreach file,$(FILES),$(file).o)

all: $(APP)

clean:
	rm -f $(APP) *.o

%.o: %.c %.h 
	gcc $(CFLAGS) -c $< -o $@

$(APP) : $(OFILES)
	gcc $(CFLAGS) $(OFILES) -o $(APP) $(OFLAGS)

