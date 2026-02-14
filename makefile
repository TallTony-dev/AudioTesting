makefile:
#Heavy llm usage here
COMPILER = clang++
CC = clang

OBJDIR = obj
CPPFILES := $(shell find src -name '*.cpp' -not -path 'src/raygui/raylib/*')
CFILES := $(shell find src -name '*.c' -not -path 'src/raygui/raylib/*' -not -path 'src/miniaudio/*')
CPP_OBJECTS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(CPPFILES))
C_OBJECTS := $(patsubst src/%.c,$(OBJDIR)/%.o,$(CFILES))
OBJECTS := $(CPP_OBJECTS) $(C_OBJECTS)

RAYLIB_DIR = src/raygui/raylib/src
RAYLIB_LIB = $(RAYLIB_DIR)/libraylib.a
RAYLIB_INCLUDE = -I$(RAYLIB_DIR) -Isrc/raygui

MACOS_OPT = -std=c++23
MACOS_FRAMEWORKS = -framework CoreAudio -framework AudioToolbox -framework CoreFoundation -framework IOKit -framework Cocoa -framework OpenGL
MACOS_OUT = -o "bin/build_osx"
WINDOWS_OPT = -std=c++23
WINDOWS_OUT = -o "bin/build_windows"

all: build_osx build_windows ##build_web

build_osx: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(RAYLIB_LIB) $(MACOS_OUT) $(MACOS_OPT) $(MACOS_FRAMEWORKS)

build_windows: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(RAYLIB_LIB) $(WINDOWS_OUT) $(WINDOWS_OPT)

$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(COMPILER) $(RAYLIB_INCLUDE) -c $< -o $@

$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(RAYLIB_INCLUDE) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) bin/*
