APP = test
FILES = gps main
CFLAGS = -std=c99 -Wno-implicit-function-declaration
CFILES = $(foreach file,$(FILES),$(file).cc)
OFILES = $(foreach file,$(FILES),$(file).o)

all: $(APP)

clean:
	rm -f $(APP) $(OFILES)

%.o: %.c %.h 
	gcc $(CFLAGS) -c $< -o $@

$(APP) : $(OFILES)
	gcc $(CFLAGS) $(OFILES) -o $(APP)

