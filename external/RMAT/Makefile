CC  = gcc
CPP = g++

TARGET = driverForRmat

CFLAGS   = -O3 -std=c99 -fopenmp
CPPFLAGS = -O3 -fopenmp

LDFLAGS  =  $(CPPFLAGS)
INCLUDES = .
LIBS     = -lgomp -lm

OBJECTS  = RngStream.o utilityFunctions.o timer.o\
          generateRMAT.o writeOutputFiles.o

all: $(TARGET) message

$(TARGET): $(OBJECTS) $(TARGET).o
	$(CPP) $(LDFLAGS) -o $(TARGET) $(TARGET).o $(OBJECTS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -I$(INCLUDES) -o $@

.cpp.o:
	$(CPP) $(CPPFLAGS) -c $< -I$(INCLUDES) -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

message:
	echo "Executable: $(TARGET) have been created"
