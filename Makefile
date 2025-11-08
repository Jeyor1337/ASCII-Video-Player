CC=gcc
CFLAGS=-g -std=gnu99
CXX=g++
CXXFLAGS=-Wall -g

# Directories
SRC_DIR=src
BUILD_DIR=build

# Interpreter settings
INTERPRETER_SRC=$(SRC_DIR)/interpreter.cpp
INTERPRETER_TARGET=$(BUILD_DIR)/interpreter
INTERPRETER_CFLAGS=$(shell pkg-config --cflags opencv4 | sed 's|-I|-isystem |g' | sed 's|//data/data/com.termux/files/usr||g')
INTERPRETER_LDFLAGS=$(shell pkg-config --libs opencv4 | sed 's|//data/data/com.termux/files/usr||g') -ljson-c

# Translator settings
TRANSLATOR_SRC=$(SRC_DIR)/translator.cpp
TRANSLATOR_TARGET=$(BUILD_DIR)/translator
TRANSLATOR_CFLAGS=$(shell pkg-config --cflags opencv4 | sed 's|-I|-isystem |g' | sed 's|//data/data/com.termux/files/usr||g')
TRANSLATOR_LDFLAGS=$(shell pkg-config --libs opencv4 | sed 's|//data/data/com.termux/files/usr||g')

TARGETS=$(INTERPRETER_TARGET) $(TRANSLATOR_TARGET)

.PHONY: all clean

all: $(BUILD_DIR) $(TARGETS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(INTERPRETER_TARGET): $(INTERPRETER_SRC) $(SRC_DIR)/ascii_decode.h
	$(CXX) $(CXXFLAGS) $(INTERPRETER_CFLAGS) -o $@ $(INTERPRETER_SRC) $(INTERPRETER_LDFLAGS)

$(TRANSLATOR_TARGET): $(TRANSLATOR_SRC) $(SRC_DIR)/ascii_decode.h
	$(CXX) $(CXXFLAGS) $(TRANSLATOR_CFLAGS) -o $@ $(TRANSLATOR_SRC) $(TRANSLATOR_LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

# Note: This Makefile requires the json-c and OpenCV 4 libraries to be installed.
# On Debian/Ubuntu, you can install them with:
#   sudo apt-get install libjson-c-dev libopencv-dev
# On Fedora/CentOS, you can install them with:
#   sudo dnf install json-c-devel opencv-devel
# On macOS (with Homebrew), you can install them with:
#   brew install json-c opencv
