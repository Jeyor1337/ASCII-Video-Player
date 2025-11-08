CC=gcc
CFLAGS=-Wall -g -std=c99
CXX=g++
CXXFLAGS=-Wall -g

# Interpreter settings
INTERPRETER_SRC=interpreter.c
INTERPRETER_TARGET=interpreter
INTERPRETER_LDFLAGS=-ljson-c

# Translator settings
TRANSLATOR_SRC=translator.cpp
TRANSLATOR_TARGET=translator
TRANSLATOR_CFLAGS=$(shell pkg-config --cflags opencv4 | sed 's|//data/data/com.termux/files/usr||g')
TRANSLATOR_LDFLAGS=$(shell pkg-config --libs opencv4 | sed 's|//data/data/com.termux/files/usr||g')

TARGETS=$(INTERPRETER_TARGET) $(TRANSLATOR_TARGET)

.PHONY: all clean

all: $(TARGETS)

$(INTERPRETER_TARGET): $(INTERPRETER_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(INTERPRETER_LDFLAGS)

$(TRANSLATOR_TARGET): $(TRANSLATOR_SRC)
	$(CXX) $(CXXFLAGS) $(TRANSLATOR_CFLAGS) -o $@ $^ $(TRANSLATOR_LDFLAGS)

clean:
	rm -f $(TARGETS)

# Note: This Makefile requires the json-c and OpenCV 4 libraries to be installed.
# On Debian/Ubuntu, you can install them with:
#   sudo apt-get install libjson-c-dev libopencv-dev
# On Fedora/CentOS, you can install them with:
#   sudo dnf install json-c-devel opencv-devel
# On macOS (with Homebrew), you can install them with:
#   brew install json-c opencv
