makefile:
COMPILER = clang++

CFILES := $(wildcard src/*.cpp src/**/*.cpp)
OBJECTS := $(CFILES:.cpp=.o)

MACOS_OPT = -std=c++23
MACOS_OUT = -o "bin/build_osx"
WINDOWS_OPT = -std=c++23
WINDOWS_OUT = -o "bin/build_windows"

all: build_osx build_windows ##build_web

build_osx: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(MACOS_OUT) $(MACOS_OPT)

build_windows: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(WINDOWS_OUT) $(WINDOWS_OPT)

%.o: %.cpp
	$(COMPILER) -c $< -o $@


clean:
	rm -rf src/*.o src/**/*.o src/*.web.o src/**/*.web.o bin/*
