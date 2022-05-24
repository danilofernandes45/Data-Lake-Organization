# Macros:
CPP= g++
CFLAGS= -O3 -fopenmp
OBJECTS= Decoder.o main.o

# Targets:
all: BRKGA_Opt

BRKGA_Opt: $(OBJECTS)
	$(CPP) $(CFLAGS) $(OBJECTS) -o BRKGA_Opt
        
Decoder.o:
	$(CPP) $(CFLAGS) -c Decoder.cpp

main.o:
	$(CPP) $(CFLAGS) -c main.cpp

# Remove:
clean:
	rm -rf BRKGA_Opt $(OBJECTS)
