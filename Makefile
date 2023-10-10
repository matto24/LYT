.PHONY: all clean

CC := g++
CFLAGS := -std=c++11 -Wno-deprecated-enum-enum-conversion -O2

# SFML libraries
SFML_LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# PortAudio libraries
PORTAUDIO_LIBS := -lportaudio -lsndfile

# WxWidget setup
WX_LIBS := $(shell wx-config --libs)
WX_CFLAGS := $(shell wx-config --cxxflags)

SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
DEPENDS := $(patsubst %.cpp,%.d,$(SOURCES))

# Executable name
EXECUTABLE := main

all: $(EXECUTABLE)

clean:
	$(RM) $(OBJECTS) $(DEPENDS) $(EXECUTABLE)

-include $(DEPENDS)

# Make object files
%.o: %.cpp Makefile
	$(CC) $(CFLAGS) $(WX_CFLAGS) -MMD -MP -c $< -o $@

# Linking the executable from the object files
$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@ $(SFML_LIBS) $(WX_LIBS) $(PORTAUDIO_LIBS)
	./$(EXECUTABLE)

