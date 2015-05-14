COMPILER = g++ -g
FILE_NAMES = p4
EXECUTABLE = program4

all: $(EXECUTABLE)

$(EXECUTABLE): $(FILE_NAMES).o
	$(COMPILER) $(FILE_NAMES).o -o $(EXECUTABLE)

$(FILE_NAMES).o: $(FILE_NAMES).cpp
	$(COMPILER) -c $(FILE_NAMES).cpp

clean:
	rm -f *.o *~ core $(EXECUTABLE)

