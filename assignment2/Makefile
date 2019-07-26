
PLATFORM := $(shell uname)
COMP = g++

# Use pkg-config to get the right libraries for your platform
GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW -lGL
EXT =
DEFS = `pkg-config --cflags glfw3`

# add -std==c++14 for home pc

# Any other platform specific libraries here...
ifneq (, $(findstring CYGWIN, $(PLATFORM)))
    GL_LIBS = -lopengl32 -lglfw3 -lglew32
	EXT = .exe
    DEFS =-DWIN32
endif

LINK += shader.o class_object.o $(EXT)

assign2 $(EXT) : main.o $(LINK)
	$(COMP) $(DEFS) -o assign2 main.o $(LINK) $(GL_LIBS)

main.o : main.cpp $(LINK)
	$(COMP) $(DEFS) -c main.cpp

shader.o : shader.cpp shader.hpp
	$(COMP) $(DEFS) -c shader.cpp

class_object.o : class_object.h class_object.cpp
	$(COMP) $(DEFS) -c class_object.cpp

clean:
	rm -f *.o assign2$(EXT)
