
PLATFORM := $(shell uname)
COMP = g++

# Use pkg-config to get the right libraries for your platform
GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW -lGL
EXT =
DEFS = `pkg-config --cflags glfw3`

# Any other platform specific libraries here...
ifneq (, $(findstring CYGWIN, $(PLATFORM)))
    GL_LIBS = -lopengl32 -lglfw3 -lglew32
	EXT = .exe
    DEFS =-DWIN32
endif

LINK += shader.o polygon_class.o $(EXT)

assign1 $(EXT) : main.o $(LINK)
	$(COMP) $(DEFS) -o assign1 main.o $(LINK) $(GL_LIBS)

main.o : main.cpp $(LINK)
	$(COMP) $(DEFS) -c main.cpp

shader.o : shader.cpp shader.hpp
	$(COMP) $(DEFS) -c shader.cpp

polygon_class.o : polygon_class.h polygon_class.cpp
	$(COMP) $(DEFS) -c polygon_class.cpp

clean:
	rm -f *.o assign1$(EXT)
