
GL_LIBS = `pkg-config --static --libs glfw3 glew` -lGLEW -lGL
EXT =
CPPFLAGS = -g `pkg-config --cflags glfw3`

CC = g++
EXE = assign3_part1
OBJS = main.o class_object.o table.o shader.o viewer.o

.PHONY: all clean

# If you haven't seen this before, notice how the rules
# of the Makefile build .o files from c++ source, and then
# build the executable from the .o files. Files are only
# re-made if they've been modified, or something they depend
# on has been modified.

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

main.o: main.cpp inputstate.h
	$(CC) $(CPPFLAGS) -c main.cpp

class_object.o : class_object.h class_object.cpp
	$(CC) $(CPPFLAGS) -c class_object.cpp

shader.o : shader.cpp shader.hpp
	$(CC) $(CPPFLAGS) -c shader.cpp

viewer.o: viewer.h viewer.cpp inputstate.h
	$(CC) $(CPPFLAGS) -c viewer.cpp

table.o: table.h table.cpp
	$(CC) $(CPPFLAGS) -c table.cpp

clean:
	rm -f *.o $(EXE)$(EXT)
