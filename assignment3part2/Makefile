
GL_LIBS = `pkg-config --static --libs glfw3 glew` -lGLEW -lGL -I external_files

EXT =
CPPFLAGS = -std=c++11 -g `pkg-config --cflags glfw3`
CC = g++
EXTPATH = external_files/
EXE = assign3_part2
OBJS = main.o class_object.o shader.o stb_image.o tiny_obj_loader.o viewer.o

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

main.o: main.cpp
	$(CC) $(CPPFLAGS) -c main.cpp

class_object.o : class_object.h class_object.cpp
	$(CC) $(CPPFLAGS) -c class_object.cpp

shader.o : $(EXTPATH)shader.cpp $(EXTPATH)shader.hpp
	$(CC) $(CPPFLAGS) -c $(EXTPATH)shader.cpp

stb_image.o : $(EXTPATH)stb_image.cpp
	$(CC) $(CPPFLAGS) -c $(EXTPATH)stb_image.cpp

#table.o: table.h table.cpp
#	$(CC) $(CPPFLAGS) -c table.cpp

tiny_obj_loader.o : $(EXTPATH)tiny_obj_loader.h $(EXTPATH)tiny_obj_loader.cc
	$(CC) $(CPPFLAGS) -c $(EXTPATH)tiny_obj_loader.cc

viewer.o: $(EXTPATH)viewer.h $(EXTPATH)viewer.cpp $(EXTPATH)inputstate.h
	$(CC) $(CPPFLAGS) -c $(EXTPATH)viewer.cpp

clean:
	rm -f *.o $(EXE)$(EXT)
